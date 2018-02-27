#include "ShadowMap.h"

ShadowMap::ShadowMap()
{
	this->device = nullptr;
	this->shadowDepthView = nullptr;
	this->shadowSRV = nullptr;
	this->shaderBlob = nullptr;
	this->errorBlob = nullptr;
	this->shadowShader_VS = nullptr;
	this->shadowShader_PS = nullptr;
	this->inp_Pos_layout = nullptr;
	this->width = -1;
	this->height = -1;

	this->layout = layout::sMap;
}

ShadowMap::~ShadowMap()
{
	if (this->errorBlob != nullptr)
	{
		this->errorBlob->Release();
	}

	this->shadowSRV->Release();
	this->shadowDepthView->Release();
	this->shadowShader_VS->Release();
	this->shadowShader_PS->Release();
	this->inp_Pos_layout->Release();
	this->pointSample->Release();
}

ID3D11SamplerState * ShadowMap::getPointSample()
{
	return this->pointSample;
}

void ShadowMap::createShadowShaders()
{
	HRESULT hr;
	hr = D3DCompileFromFile(L"Shadow_VS.hlsl", nullptr, nullptr, "VS_Entry", "vs_5_0", D3DCOMPILE_DEBUG, 0, &this->shaderBlob, &this->errorBlob);

	if (FAILED(hr))
	{
		exit(-1);
	}

	hr = this->device->CreateVertexShader(this->shaderBlob->GetBufferPointer(), this->shaderBlob->GetBufferSize(), NULL, &this->shadowShader_VS);

	if (FAILED(hr))
	{
		exit(-1);
	}


	D3D11_INPUT_ELEMENT_DESC dscPOS[] = {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0};
	hr = this->device->CreateInputLayout(dscPOS, ARRAYSIZE(dscPOS), this->shaderBlob->GetBufferPointer(), this->shaderBlob->GetBufferSize(), &this->inp_Pos_layout);
	if (FAILED(hr))
	{
		exit(-1);
	}
	
	//release blob shader is finnished
	this->shaderBlob->Release();


	//pixelShader
	hr = D3DCompileFromFile(L"Shadow_PS.hlsl", nullptr, nullptr, "PS_Entry", "ps_5_0", D3DCOMPILE_DEBUG, 0, &this->shaderBlob, &this->errorBlob);

	if (FAILED(hr))
	{
		exit(-1);
	}

	hr = this->device->CreatePixelShader(this->shaderBlob->GetBufferPointer(), this->shaderBlob->GetBufferSize(), NULL, &this->shadowShader_PS);

	if (FAILED(hr))
	{
		exit(-1);
	}

	//release blob shader is finnished
	this->shaderBlob->Release();
}

void ShadowMap::createShadowDepthView()
{
	HRESULT hr;
	ID3D11Texture2D * texture;
	
	//create 2D Texture
	D3D11_TEXTURE2D_DESC dscTexture;
	ZeroMemory(&dscTexture, sizeof(dscTexture));
	dscTexture.Width = this->width;
	dscTexture.Height = this->height;
	dscTexture.Format = DXGI_FORMAT_R24G8_TYPELESS;
	dscTexture.Usage = D3D11_USAGE_DEFAULT;
	dscTexture.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	dscTexture.MipLevels = 1;
	dscTexture.ArraySize = 1;
	dscTexture.CPUAccessFlags = 0;
	dscTexture.MiscFlags = 0;
	dscTexture.SampleDesc.Count = 1;
	dscTexture.SampleDesc.Quality = 0;

	hr = this->device->CreateTexture2D(&dscTexture, nullptr, &texture);
	if (FAILED(hr))
	{
		exit(-1);
	}

	//create DepthStencilView
	D3D11_DEPTH_STENCIL_VIEW_DESC dscDepthView;
	ZeroMemory(&dscDepthView, sizeof(dscDepthView));
	dscDepthView.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dscDepthView.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dscDepthView.Texture2D.MipSlice = 0;
	dscDepthView.Flags = 0;
	hr = this->device->CreateDepthStencilView(texture, &dscDepthView, &this->shadowDepthView);
	if (FAILED(hr))
	{
		exit(-1);
	}

	//create shadow SRV
	D3D11_SHADER_RESOURCE_VIEW_DESC dscSRV;
	ZeroMemory(&dscSRV, sizeof(dscSRV));
	dscSRV.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	dscSRV.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	dscSRV.Texture2D.MipLevels = 1;
	hr = this->device->CreateShaderResourceView(texture, &dscSRV, &this->shadowSRV);
	if (FAILED(hr))
	{
		exit(-1);
	}

	//release com object
	texture->Release();
}

void ShadowMap::shadowMapSampler()
{
	HRESULT hr;
	D3D11_SAMPLER_DESC sampler_description;
	sampler_description.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	sampler_description.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampler_description.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampler_description.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampler_description.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampler_description.MipLODBias = 0.0f;
	sampler_description.MaxAnisotropy = 1;

 	hr = this->device->CreateSamplerState(&sampler_description, &this->pointSample);
}

ID3D11DepthStencilView * ShadowMap::getDepthShadowView()
{
	return this->shadowDepthView;
}

ID3D11ShaderResourceView * ShadowMap::getShadowSRV()
{
	return this->shadowSRV;
}

ID3D11InputLayout * ShadowMap::getInputLayout()
{
	return this->inp_Pos_layout;
}

ID3D11VertexShader * ShadowMap::getVertexShader()
{
	return this->shadowShader_VS;
}

ID3D11PixelShader * ShadowMap::getPixelShader()
{
	return this->shadowShader_PS;
}

int ShadowMap::getLayout()
{
	return this->layout;
}

void ShadowMap::setWidthHeight(int widht, int height)
{
	this->width = widht;
	this->height = height;
}

void ShadowMap::setDevice(ID3D11Device * in_device)
{
	this->device = in_device;
}
