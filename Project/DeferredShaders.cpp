#include "DeferredShaders.h"

DeferredShaders::DeferredShaders()
{
	//set nullptrs
	//todo:
	this->error_blob = nullptr;
	this->shader_blob = nullptr;
	this->device = nullptr;
}

DeferredShaders::~DeferredShaders()
{

	//RELEASE THE KRAKENS(com-objects)
	if (this->error_blob != nullptr)
	{
		this->error_blob->Release();
	}

	this->GeometryPass_VS->Release();
	this->LightPass_PS->Release();
	this->LightPass_VS->Release();
	this->GeometryPassTerrain_PS->Release();
	this->GeometryPassBox_PS->Release();
	this->geometry_Shader->Release();
	this->GeometryPassPlane_GS->Release();
	this->GeometryPassPlane_PS->Release();
	this->GeometryPassPlane_VS->Release();
	this->inp_Pos_layout->Release();
	this->inp_PTN_layout->Release();
	this->inp_PT_layout->Release();
}

ID3D11VertexShader * DeferredShaders::getGeometryPassVS() const
{
	return this->GeometryPass_VS;
}

ID3D11PixelShader * DeferredShaders::getGeometryTerrainPS() const
{
	return this->GeometryPassTerrain_PS;
}

ID3D11PixelShader * DeferredShaders::getGeometryBoxPS() const
{
	return this->GeometryPassBox_PS;
}

ID3D11PixelShader * DeferredShaders::getGeoemtryPlanePS() const
{
	return this->GeometryPassPlane_PS;
}

ID3D11VertexShader * DeferredShaders::getGeometryPlaneVS() const
{
	return this->GeometryPassPlane_VS;
}

ID3D11GeometryShader * DeferredShaders::getGeomtryPlaneGS() const
{
	return this->GeometryPassPlane_GS;
}

ID3D11VertexShader * DeferredShaders::getLightVS() const
{
	return this->LightPass_VS;
}

ID3D11PixelShader * DeferredShaders::getLightPS() const
{
	return this->LightPass_PS;
}

ID3D11GeometryShader * DeferredShaders::getGeoShader() const
{
	return this->geometry_Shader;
}

ID3D11InputLayout * DeferredShaders::getPTLayout() const
{
	return this->inp_PT_layout;
}

ID3D11InputLayout * DeferredShaders::getPosLayout() const
{
	return this->inp_Pos_layout;
}

ID3D11InputLayout * DeferredShaders::getPTNLayout() const
{
	return this->inp_PTN_layout;
}

void DeferredShaders::setDevice(ID3D11Device * in_device)
{
	this->device = in_device;
}

void DeferredShaders::compileVertexShaders()
{
	HRESULT hr;


	//create vertex shader for geometry pass
	hr = D3DCompileFromFile(	L"Geometry_Pass_VS.hlsl",
										nullptr,
										nullptr,
										"VS_Entry",
										"vs_5_0",
										D3DCOMPILE_DEBUG,
										0,
										&this->shader_blob,
										&this->error_blob);

	if (FAILED(hr))
	{
		exit(-1);
	}

	hr = this->device->CreateVertexShader(	this->shader_blob->GetBufferPointer(),
											this->shader_blob->GetBufferSize(),
											NULL,
											&this->GeometryPass_VS);

	if (FAILED(hr))
	{
		exit(-1);
	}

	D3D11_INPUT_ELEMENT_DESC dscPTN[] = {
										{
											"POSITION",
											0,
											DXGI_FORMAT_R32G32B32_FLOAT,
											0,
											0,
											D3D11_INPUT_PER_VERTEX_DATA,
											0
										},
										{
											"NORMAL",
											0,
											DXGI_FORMAT_R32G32B32_FLOAT,
											0,
											12,
											D3D11_INPUT_PER_VERTEX_DATA,
											0
										},
										{
											"TEXCOORD",
											0,
											DXGI_FORMAT_R32G32_FLOAT,
											0,
											24,
											D3D11_INPUT_PER_VERTEX_DATA,
											0
										}
										};
	hr = this->device->CreateInputLayout(dscPTN, ARRAYSIZE(dscPTN), this->shader_blob->GetBufferPointer(), this->shader_blob->GetBufferSize(), &this->inp_PTN_layout);

	this->shader_blob->Release();


	//create vertex shader for light pass

	hr = D3DCompileFromFile(L"Light_VS.hlsl",
							nullptr,
							nullptr,
							"VS_Entry",
							"vs_5_0",
							D3DCOMPILE_DEBUG,
							0,
							&this->shader_blob,
							&this->error_blob);

	if (FAILED(hr))
	{
		exit(-1);
	}

	hr = this->device->CreateVertexShader(	this->shader_blob->GetBufferPointer(),
											this->shader_blob->GetBufferSize(),
											NULL,
											&this->LightPass_VS);

	if (FAILED(hr))
	{
		exit(-1);
	}

	D3D11_INPUT_ELEMENT_DESC dscPos[] = {
										{
											"POSITION",
											0,
											DXGI_FORMAT_R32G32B32_FLOAT,
											0,
											0,
											D3D11_INPUT_PER_VERTEX_DATA,
											0
										},
										};

	hr = this->device->CreateInputLayout(dscPos, ARRAYSIZE(dscPos), this->shader_blob->GetBufferPointer(), this->shader_blob->GetBufferSize(), &this->inp_Pos_layout);

	this->shader_blob->Release();

	//create vertex shader for plane geometrypass

	hr = D3DCompileFromFile(L"GeometryPassPlane_VS.hlsl",
							nullptr,
							nullptr,
							"VS_Entry",
							"vs_5_0",
							D3DCOMPILE_DEBUG,
							0,
							&this->shader_blob,
							&this->error_blob);

	if (FAILED(hr))
	{
		exit(-1);
	}

	hr = this->device->CreateVertexShader(	this->shader_blob->GetBufferPointer(),
											this->shader_blob->GetBufferSize(),
											NULL,
											&this->GeometryPassPlane_VS);

	if (FAILED(hr))
	{
		exit(-1);
	}

		D3D11_INPUT_ELEMENT_DESC dscPT[] = {
											{
												"POSITION",
												0,
												DXGI_FORMAT_R32G32B32_FLOAT,
												0,
												0,
												D3D11_INPUT_PER_VERTEX_DATA,
												0
											},
											{
												"TEXCOORD",
												0,
												DXGI_FORMAT_R32G32_FLOAT,
												0,
												12,
												D3D11_INPUT_PER_VERTEX_DATA,
												0
											},
											};

	hr = this->device->CreateInputLayout(dscPT, ARRAYSIZE(dscPT), this->shader_blob->GetBufferPointer(), this->shader_blob->GetBufferSize(), &this->inp_PT_layout);


	if (FAILED(hr))
	{
		exit(-1);
	}


	this->shader_blob->Release();
}

void DeferredShaders::compilePixelShaders()
{
	HRESULT hr;
	
	//Create Geometry pass pixel shader 
	hr = D3DCompileFromFile(L"GeometryPassTerrain_PS.hlsl",
							nullptr,
							nullptr,
							"PS_Entry",
							"ps_5_0",
							D3DCOMPILE_DEBUG,
							0,
							&this->shader_blob,
							&this->error_blob);

	if (FAILED(hr))
	{
		exit(-1);
	}

	hr = this->device->CreatePixelShader	(this->shader_blob->GetBufferPointer(),
											this->shader_blob->GetBufferSize(),
											NULL,
											&this->GeometryPassTerrain_PS);
	
	if (FAILED(hr))
	{
		exit(-1);
	}

	this->shader_blob->Release();

	hr = D3DCompileFromFile(L"GeometryPassBox_PS.hlsl",
							nullptr,
							nullptr,
							"PS_Entry",
							"ps_5_0",
							D3DCOMPILE_DEBUG,
							0,
							&this->shader_blob,
							&this->error_blob);

	if (FAILED(hr))
	{
		exit(-1);
	}

	hr = this->device->CreatePixelShader(	this->shader_blob->GetBufferPointer(),
											this->shader_blob->GetBufferSize(),
											NULL,
											&this->GeometryPassBox_PS);

	if (FAILED(hr))
	{
		exit(-1);
	}

	this->shader_blob->Release();

	//create light pass pixel shaders
	hr = D3DCompileFromFile(L"LightPass_PS.hlsl",
							nullptr,
							nullptr,
							"PS_Entry",
							"ps_5_0",
							D3DCOMPILE_DEBUG,
							0,
							&this->shader_blob,
							&this->error_blob);

	if (FAILED(hr))
	{
		exit(-1);
	}

	hr = this->device->CreatePixelShader(	this->shader_blob->GetBufferPointer(),
											this->shader_blob->GetBufferSize(),
											NULL,
											&this->LightPass_PS);

	if (FAILED(hr))
	{
		exit(-1);
	}

	this->shader_blob->Release();

	//create plane pixel shader
	hr = D3DCompileFromFile(L"GeometryPassPlane_PS.hlsl",
							nullptr,
							nullptr,
							"PS_Entry",
							"ps_5_0",
							D3DCOMPILE_DEBUG,
							0,
							&this->shader_blob,
							&this->error_blob);

	if (FAILED(hr))
	{
		exit(-1);
	}

	hr = this->device->CreatePixelShader(	this->shader_blob->GetBufferPointer(),
											this->shader_blob->GetBufferSize(),
											NULL,
											&this->GeometryPassPlane_PS);

	if (FAILED(hr))
	{
		exit(-1);
	}

	this->shader_blob->Release();
}

void DeferredShaders::compileGeometryShaders()
{
	HRESULT hr;
	hr = D3DCompileFromFile(L"Geometry_Shader.hlsl",
							nullptr,
							nullptr,
							"GS_Entry",
							"gs_5_0",
							D3DCOMPILE_DEBUG,
							0,
							&this->shader_blob,
							&this->error_blob);

	if (FAILED(hr))
	{
		exit(-1);
	}

	hr = this->device->CreateGeometryShader(this->shader_blob->GetBufferPointer(),
											this->shader_blob->GetBufferSize(),
											NULL,
											&this->geometry_Shader);

	if (FAILED(hr))
	{
		exit(-1);
	}

	this->shader_blob->Release();

	hr = D3DCompileFromFile(L"GeometryPassPlane_GS.hlsl",
							nullptr,
							nullptr,
							"GS_Entry",
							"gs_5_0",
							D3DCOMPILE_DEBUG,
							0,
							&this->shader_blob,
							&this->error_blob);

	if (FAILED(hr))
	{
		exit(-1);
	}

	hr = this->device->CreateGeometryShader(this->shader_blob->GetBufferPointer(),
											this->shader_blob->GetBufferSize(),
											NULL,
											&this->GeometryPassPlane_GS);

	if (FAILED(hr))
	{
		exit(-1);
	}

	this->shader_blob->Release();
}
