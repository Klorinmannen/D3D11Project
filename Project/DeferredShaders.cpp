#include "DeferredShaders.h"

DeferredShaders::DeferredShaders()
{
	//set nullptrs
	//todo:
}

DeferredShaders::~DeferredShaders()
{

	//RELEASE THE KRAKENS(com-objects)
}

void DeferredShaders::createVertexShaders()
{
	HRESULT hresult;

	this->compileGeometryPassShader(DeferredShaders::Vertex_S);
	hresult = this->device->CreateVertexShader(this->shader_blob->GetBufferPointer(),
												this->shader_blob->GetBufferSize(),
												NULL,
												&this->geometry_vertex_shader);
	
	if (FAILED(hresult))
	{
		exit(-1);
	}
	this->createInputLayout(layout::PTN);
	this->shader_blob->Release();

	this->compileLightPassShader(DeferredShaders::Vertex_S);
	hresult = this->device->CreateVertexShader(this->shader_blob->GetBufferPointer(),
												this->shader_blob->GetBufferSize(),
												NULL,
												&this->light_vertex_shader);

	if (FAILED(hresult))
	{
		exit(-1);
	}

	this->createInputLayout(layout::Pos);
	this->shader_blob->Release();
}

void DeferredShaders::createPixelShaders()
{
	HRESULT hresult;

	this->compileGeometryPassShader(DeferredShaders::Pixel_S);
	hresult = this->device->CreatePixelShader(this->shader_blob->GetBufferPointer(), 
												this->shader_blob->GetBufferSize(),
												NULL, 
												&this->geometry_pixel_shader);
	if (FAILED(hresult))
	{
		exit(-1);
	}
	this->shader_blob->Release();

	this->compileLightPassShader(DeferredShaders::Pixel_S);
	hresult = this->device->CreatePixelShader(this->shader_blob->GetBufferPointer(),
												this->shader_blob->GetBufferSize(),
												NULL,
												&this->light_pixel_shader);
	if (FAILED(hresult))
	{
		exit(-1);
	}
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

ID3D11GeometryShader * DeferredShaders::getGeoShader() const
{
	return this->geometry_Shader;
}

ID3D11InputLayout * DeferredShaders::getPNLayout() const
{
	return this->inp_PN_layout;
}

ID3D11InputLayout * DeferredShaders::getPosLayout() const
{
	return this->inp_Pos_layout;
}

ID3D11InputLayout * DeferredShaders::getPTNLayout() const
{
	return this->inp_PTN_layout;
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

void DeferredShaders::setDevice(ID3D11Device * in_device)
{
	this->device = in_device;
}

void DeferredShaders::compileGeometryPassShader(type in_type)
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

void DeferredShaders::compileLightPassShader(type in_type)
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

void DeferredShaders::createInputLayout(int in_type)
{
	HRESULT hr;
	switch (in_type)
	{
	//case layout::PN:
	//{
	//	D3D11_INPUT_ELEMENT_DESC dscPN[] = {
	//										{
	//											"POSITION",
	//											0,
	//											DXGI_FORMAT_R32G32B32_FLOAT,
	//											0,
	//											0,
	//											D3D11_INPUT_PER_VERTEX_DATA,
	//											0
	//										},
	//										{
	//											"NORMAL",
	//											0,
	//											DXGI_FORMAT_R32G32B32_FLOAT,
	//											0,
	//											12,
	//											D3D11_INPUT_PER_VERTEX_DATA,
	//											0
	//										}
	//										};

	//hr = this->device->CreateInputLayout(dscPN, ARRAYSIZE(dscPN), this->shader_blob->GetBufferPointer(), this->shader_blob->GetBufferSize(), &this->inp_PN_layout);
	//	break;
	//}

	
	case layout::Pos:
	{
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
		break;
	}
	case layout::PTN:
	{
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
		break;
	}
	}

}

void DeferredShaders::createGeometryShader()
{
	HRESULT hresult;

	this->compileGeometryShader();
	hresult = this->device->CreateGeometryShader(this->shader_blob->GetBufferPointer(), 
													this->shader_blob->GetBufferSize(), 
													NULL, 
													&this->geometry_Shader);
	if (FAILED(hresult))
	{
		exit(-1);
	}
	this->shader_blob->Release();

}

void DeferredShaders::compileGeometryShader()
{
	this->hResult = D3DCompileFromFile(
										L"Geometry_Shader.hlsl",
										nullptr,
										nullptr,
										"GS_Entry",
										"gs_5_0",
										D3DCOMPILE_DEBUG,
										0,
										&this->shader_blob,
										&this->error_blob
	);
}
