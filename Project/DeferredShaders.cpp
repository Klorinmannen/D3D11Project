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
	this->createInputLayout();
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
	this->shader_blob->Release();

	this->compileLightShader(DeferredShaders::Vertex_S);
	this->device->CreateVertexShader(this->shader_blob->GetBufferPointer(),
									this->shader_blob->GetBufferSize(),
									NULL,
									&this->light_vertex_shader);
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

ID3D11InputLayout * DeferredShaders::getPTNLayout() const
{
	return this->inp_PTN_layout;
}

ID3D11InputLayout * DeferredShaders::getPNLayout() const
{
	return this->inp_PN_layout;
}

ID3D11InputLayout * DeferredShaders::getPCLayout() const
{
	return this->inp_PC_layout;
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
											L"Deferred_geometry_VS.hlsl", 
											nullptr,		
											nullptr,		
											"Deferred_VS_Entry",		
											"vs_5_0",		
											D3DCOMPILE_DEBUG,	
											0,				
											&this->shader_blob,				
											&this->error_blob		
		);
		break;
	case DeferredShaders::Pixel_S:
		this->hResult = D3DCompileFromFile(
											L"Deferred_geometry_PS.hlsl",
											nullptr,
											nullptr,
											"Deferred_PS_Entry",
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
		this->hResult = D3DCompileFromFile( L"Deferred_light_VS.hlsl",
											nullptr,
											nullptr,
											"Deferred_VS_Entry",
											"vs_5_0",
											D3DCOMPILE_DEBUG,
											0,
											&this->shader_blob,
											&this->error_blob
		);
		break;
	case DeferredShaders::Pixel_S:
		this->hResult = D3DCompileFromFile( L"Deferred_light_PS.hlsl",
											nullptr,
											nullptr,
											"Deferred_PS_Entry",
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
	//Pos-Tex-Normal layout
	D3D11_INPUT_ELEMENT_DESC inp_PTN_desc[] = {
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
											{
												"NORMAL",
												0,
												DXGI_FORMAT_R32G32B32_FLOAT,
												0,
												20,
												D3D11_INPUT_PER_VERTEX_DATA,
											}

										};
	this->ptn_size = 8.0f;
	this->device->CreateInputLayout(inp_PTN_desc,
									ARRAYSIZE(inp_PTN_desc),
									this->shader_blob, 
									sizeof(this->shader_blob), 
									&this->inp_PTN_layout);
	
	//Pos-Normal layout
	D3D11_INPUT_ELEMENT_DESC inp_PN_desc[] = {
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
											}
										};
	this->pn_size = 6.0f;
	this->device->CreateInputLayout(inp_PN_desc,
									ARRAYSIZE(inp_PN_desc),
									this->shader_blob,
									sizeof(this->shader_blob),
									&this->inp_PN_layout);

	D3D11_INPUT_ELEMENT_DESC inp_PC_desc[] = {
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
												"COLOR",
												0,
												DXGI_FORMAT_R32G32B32A32_FLOAT,
												0,
												12,
												D3D11_INPUT_PER_VERTEX_DATA,
											}
	};
	this->pc_size = 6.0f;
	this->device->CreateInputLayout(inp_PC_desc,
									ARRAYSIZE(inp_PC_desc),
									this->shader_blob,
									sizeof(this->shader_blob),
									&this->inp_PC_layout);

}
