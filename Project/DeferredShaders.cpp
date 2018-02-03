#include "DeferredShaders.h"

DeferredShaders::DeferredShaders(ID3D11Device *& in_device)
{
	//Allt skapas när DeferredShaders-Objektet kommer till liv :D
	this->device = in_device;
	this->createVertexShader();
	if (FAILED(this->hResult))
	{
		//deliver krakens messages of destructive powers inherited from the ancient times of boat swallowing
	}
	this->createInputLayout();
	this->createPixelShader();
	if (FAILED(this->hResult))
	{

	}
}

DeferredShaders::~DeferredShaders()
{

	//RELEASE THE KRAKENS(com-objects)
}

void DeferredShaders::createVertexShader()
{

	this->compileShader(DeferredShaders::Vertex_S);
	this->device->CreateVertexShader(this->shader_blob->GetBufferPointer(), 
									this->shader_blob->GetBufferSize(),
									NULL, 
									&this->vertex_shader);
	this->shader_blob->Release();
}

void DeferredShaders::createPixelShader()
{
	this->compileShader(DeferredShaders::Pixel_S);
	this->device->CreatePixelShader(this->shader_blob->GetBufferPointer(), 
									this->shader_blob->GetBufferSize(),
									NULL, 
									&this->pixel_shader);
	this->shader_blob->Release();
}

const ID3D11VertexShader * DeferredShaders::getVS()
{
	return this->vertex_shader;
}

const ID3D11PixelShader * DeferredShaders::getPS()
{
	return this->pixel_shader;
}

const ID3D11InputLayout * DeferredShaders::getPTNLayout()
{
	return this->PTN_layout;
}

const ID3D11InputLayout * DeferredShaders::getPNLayout()
{
	return this->PN_layout;
}

void DeferredShaders::compileShader(type in_type)
{
	switch (in_type)
	{
	case DeferredShaders::Vertex_S:
		this->hResult = D3DCompileFromFile(
											L"Deferred_VS.hlsl", 
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
											L"Deferred_PS.hlsl",
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
	D3D11_INPUT_ELEMENT_DESC PTN_desc[] = {
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

	this->device->CreateInputLayout(PTN_desc, 
									ARRAYSIZE(PTN_desc), 
									this->shader_blob, 
									sizeof(this->shader_blob), 
									&this->PTN_layout);
	
	//Pos-Normal layout
	D3D11_INPUT_ELEMENT_DESC PN_desc[] = { 
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

	this->device->CreateInputLayout(PN_desc,
									ARRAYSIZE(PN_desc),
									this->shader_blob,
									sizeof(this->shader_blob),
									&this->PN_layout);


}
