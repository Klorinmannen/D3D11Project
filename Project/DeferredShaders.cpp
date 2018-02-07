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

	this->compileGeometryShader(DeferredShaders::Vertex_S);
	this->device->CreateVertexShader(this->shader_blob->GetBufferPointer(),
		this->shader_blob->GetBufferSize(),
		NULL,
		&this->geometry_vertex_shader);

	this->createInputLayout(layout::PN);
	this->shader_blob->Release();

	this->compileLightShader(DeferredShaders::Vertex_S);
	this->device->CreateVertexShader(this->shader_blob->GetBufferPointer(),
									this->shader_blob->GetBufferSize(),
									NULL,
									&this->light_vertex_shader);


	this->createInputLayout(layout::Pos);
	this->shader_blob->Release();
}

void DeferredShaders::createPixelShaders()
{
	HRESULT hresult;

	this->compileGeometryShader(DeferredShaders::Pixel_S);
	hresult = this->device->CreatePixelShader(this->shader_blob->GetBufferPointer(), 
									this->shader_blob->GetBufferSize(),
									NULL, 
									&this->geometry_pixel_shader);
	if (FAILED(hresult))
	{
		exit(-1);
	}
	this->shader_blob->Release();

	this->compileLightShader(DeferredShaders::Pixel_S);
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

ID3D11InputLayout * DeferredShaders::getPNLayout() const
{
	return this->inp_PN_layout;
}

ID3D11InputLayout * DeferredShaders::getPosLayout() const
{
	return this->inp_Pos_layout;
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

void DeferredShaders::createInputLayout(int in_type)
{
	switch (in_type)
	{
	case layout::PN:
	{
		D3D11_INPUT_ELEMENT_DESC dscPN[] = {
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

		this->device->CreateInputLayout(dscPN, ARRAYSIZE(dscPN), this->shader_blob->GetBufferPointer(), this->shader_blob->GetBufferSize(), &this->inp_PN_layout);
	}
		break;
	
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

		this->device->CreateInputLayout(dscPos, ARRAYSIZE(dscPos), this->shader_blob->GetBufferPointer(), this->shader_blob->GetBufferSize(), &this->inp_Pos_layout);
		break;
	}
	}

}
