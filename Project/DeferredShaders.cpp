#include "DeferredShaders.h"

DeferredShaders::DeferredShaders(ID3D11Device * in_device)
{
	//Allt skapas när DeferredShaders-Objektet kommer till liv :D
	this->device = in_device;

	this->createVertexShaders();
	if (FAILED(this->hResult))
	{
		//deliver krakens messages of destructive powers inherited from the ancient times of boat swallowing
	}
	this->createPixelShaders();
	if (FAILED(this->hResult))
	{

	}
}

DeferredShaders::~DeferredShaders()
{

	//RELEASE THE KRAKENS(com-objects)
}

void DeferredShaders::createVertexShaders()
{

	this->compileGeometryShader(DeferredShaders::Vertex_S);
	this->device->CreateVertexShader(this->shader_blob->GetBufferPointer(),
		this->shader_blob->GetBufferSize(),
		NULL,
		&this->geometry_vertex_shader);

	this->createInputLayout();
	this->shader_blob->Release();

	this->compileLightShader(DeferredShaders::Vertex_S);
	this->device->CreateVertexShader(this->shader_blob->GetBufferPointer(),
									this->shader_blob->GetBufferSize(),
									NULL,
									&this->light_vertex_shader);


	this->createInputLayout();
	this->shader_blob->Release();
}

void DeferredShaders::createPixelShaders()
{
	this->compileGeometryShader(DeferredShaders::Pixel_S);
	this->device->CreatePixelShader(this->shader_blob->GetBufferPointer(), 
									this->shader_blob->GetBufferSize(),
									NULL, 
									&this->geometry_pixel_shader);
	this->shader_blob->Release();

	this->compileLightShader(DeferredShaders::Pixel_S);
	this->device->CreatePixelShader(this->shader_blob->GetBufferPointer(),
									this->shader_blob->GetBufferSize(),
									NULL,
									&this->light_pixel_shader);
	this->shader_blob->Release();
}

ID3D11VertexShader * DeferredShaders::getGeoVS() 
{
	return this->geometry_vertex_shader;
}

ID3D11PixelShader * DeferredShaders::getGeoPS() const
{
	return this->geometry_pixel_shader;
}

ID3D11VertexShader * DeferredShaders::getLightVS() const
{
	return this->light_vertex_shader;
}

ID3D11PixelShader * DeferredShaders::getLightPS() const
{
	return this->light_pixel_shader;
}

ID3D11InputLayout * DeferredShaders::getPNLayout() const
{
	return this->inp_PN_layout;
}

float DeferredShaders::getPTNSize() const
{
	return this->ptn_size;
}

float DeferredShaders::getPNSize() const
{
	return this->pn_size;
}

float DeferredShaders::getPCSize() const
{
	return this->pc_size;
}

void DeferredShaders::compileGeometryShader(type in_type)
{
	switch (in_type)
	{
	case DeferredShaders::Vertex_S:
		this->hResult = D3DCompileFromFile(
											L"Geometry_VS.hlsl", 
											nullptr,		
											nullptr,		
											"VS_Entry",		
											"vs_5_0",		
											D3DCOMPILE_DEBUG,	
											0,				
											&this->shader_blob,				
											&this->error_blob		
		);
		break;
	case DeferredShaders::Pixel_S:
		this->hResult = D3DCompileFromFile(
											L"Geometry_PS.hlsl",
											nullptr,
											nullptr,
											"PS_Entry",
											"ps_5_0",
											D3DCOMPILE_DEBUG,
											0,
											&this->shader_blob,
											&this->error_blob
		);
		break;
	}
}

void DeferredShaders::compileLightShader(type in_type)
{
	switch (in_type)
	{
	case DeferredShaders::Vertex_S:
		this->hResult = D3DCompileFromFile( L"Light_VS.hlsl",
											nullptr,
											nullptr,
											"VS_Entry",
											"vs_5_0",
											D3DCOMPILE_DEBUG,
											0,
											&this->shader_blob,
											&this->error_blob
		);
		break;
	case DeferredShaders::Pixel_S:
		this->hResult = D3DCompileFromFile( L"Light_PS.hlsl",
											nullptr,
											nullptr,
											"PS_Entry",
											"ps_5_0",
											D3DCOMPILE_DEBUG,
											0,
											&this->shader_blob,
											&this->error_blob
		);
		break;
	}
}

void DeferredShaders::createInputLayout()
{
	D3D11_INPUT_ELEMENT_DESC dsc[] = {
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
										}
	};

	this->device->CreateInputLayout(dsc, ARRAYSIZE(dsc), this->shader_blob->GetBufferPointer(), this->shader_blob->GetBufferSize(), &this->inp_PN_layout);
}
