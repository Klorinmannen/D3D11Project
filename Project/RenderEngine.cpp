#include "RenderEngine.h"

bool RenderEngine::initiateEngine(HWND handle)
{
	/* 
	create swapchain
	create device
	create context
	*/

	DXGI_SWAP_CHAIN_DESC bb_desc;
	ZeroMemory(&bb_desc, sizeof(bb_desc));

	bb_desc.BufferCount = 1;
	bb_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	bb_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	bb_desc.OutputWindow = handle;
	bb_desc.SampleDesc.Count = 1;
	bb_desc.Windowed = TRUE;

	HRESULT hresult = D3D11CreateDeviceAndSwapChain(NULL,
													D3D_DRIVER_TYPE_HARDWARE,
													NULL,
													D3D11_CREATE_DEVICE_DEBUG,
													NULL,
													NULL,
													D3D11_SDK_VERSION,
													&bb_desc,
													&this->swapChain,
													&this->device,
													NULL,
													&this->deviceContext);
	if (FAILED(hresult))
	{
		return false;
	}

	//MSDN example
	//used to set the backbuffer at the OMS stage
	this->swapChain->GetBuffer(0, __uuidof(this->back_buffer_texture), reinterpret_cast<void**>(&this->back_buffer_texture));
	hresult = this->device->CreateRenderTargetView(this->back_buffer_texture, NULL, &this->back_buffer_view);
	if (FAILED(hresult))
	{
		exit(-1);
	}

	if (!this->setupDepthStencilBuffer())
	{
		exit(-1);
	}

	if (!this->setupRTVs())
	{
		//if shit goes haywire
		exit(-1);
	}
	this->setupVP();
	this->setupRasterizer();
	this->setupOMS();

	return true;
}

bool RenderEngine::setupRTVs()
{
	//render&shader views
	D3D11_TEXTURE2D_DESC texture_desc;
	D3D11_RENDER_TARGET_VIEW_DESC rtv_desc;
	D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;


	//wnds msgs
	HRESULT hResult;

	//create textures
	// https://msdn.microsoft.com/en-us/library/windows/desktop/ff476253(v=vs.85).aspx << D3D11_TEXTURE2D_DESC
	ZeroMemory(&texture_desc, sizeof(texture_desc));
	texture_desc.Width = this->WIDTH;
	texture_desc.Height = this->HEIGHT;
	texture_desc.MipLevels = 1;
	texture_desc.ArraySize = 1; //one texture in the array
	texture_desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; // https://msdn.microsoft.com/en-us/library/windows/desktop/bb173059(v=vs.85).aspx << DXGI_FORMAT
	texture_desc.SampleDesc.Count = 1; // https://msdn.microsoft.com/en-us/library/windows/desktop/bb173072(v=vs.85).aspx << DXGI_SAMPLE_DESC
	texture_desc.Usage = D3D11_USAGE_DEFAULT;
	texture_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE; //Use as render targer and shader resource
	texture_desc.CPUAccessFlags = 0;
	texture_desc.MiscFlags = 0;

	//create all necessary textures (normals, position, texture color)
	for (size_t i = 0; i < this->VIEW_COUNT; i++)
	{
		hResult = this->device->CreateTexture2D(&texture_desc, NULL, &this->RTTextures[i]);
		if (FAILED(hResult))
		{
			return false;
		}
	}


	//create render target views
	//https://msdn.microsoft.com/en-us/library/windows/desktop/ff476201(v=vs.85).aspx << D3D11_RENDER_TARGET_VIEW_DESC
	ZeroMemory(&rtv_desc, sizeof(rtv_desc));
	rtv_desc.Format = texture_desc.Format;
	rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtv_desc.Texture2D.MipSlice = 0; // https://msdn.microsoft.com/en-us/library/windows/desktop/ff476244(v=vs.85).aspx << D3D11_TEX2D_RTV

	for (size_t i = 0; i < this->VIEW_COUNT; i++)
	{
		hResult = this->device->CreateRenderTargetView(this->RTTextures[i], &rtv_desc, &this->RTViews[i]);
		if (FAILED(hResult))
		{
			return false;
		}
	}

	//create shader resource views
	//https://msdn.microsoft.com/en-us/library/windows/desktop/ff476211(v=vs.85).aspx << D3D11_SHADER_RESOURCE_VIEW_DESC
	ZeroMemory(&srv_desc, sizeof(srv_desc));
	srv_desc.Format = texture_desc.Format;
	srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srv_desc.Texture2D.MipLevels = 1; // https://msdn.microsoft.com/en-us/library/windows/desktop/ff476245(v=vs.85).aspx << D3D11_TEX2D_SRV
	srv_desc.Texture2D.MostDetailedMip = 0;

	for (size_t i = 0; i < this->VIEW_COUNT; i++)
	{
		hResult = this->device->CreateShaderResourceView(this->RTTextures[i], &srv_desc, &this->SRViews[i]);
		if (FAILED(hResult))
		{
			return false;
		}
	}

	//https://msdn.microsoft.com/en-us/library/windows/desktop/ff476519(v=vs.85).aspx << CreateShaderResourceView
	//https://msdn.microsoft.com/en-us/library/windows/desktop/ff476517(v=vs.85).aspx << CreateRenderTargetView
	//https://msdn.microsoft.com/en-us/library/windows/desktop/ff476521(v=vs.85).aspx << CreateTexture2D

	return true;
}

void RenderEngine::setupVP()
{
	//https://msdn.microsoft.com/en-us/library/windows/desktop/ff476260(v=vs.85).aspx << D3D11_VIEWPORT

	this->view_port.Height = static_cast<float>(this->HEIGHT);
	this->view_port.Width = static_cast<float>(this->WIDTH);
	this->view_port.MinDepth = 0.0f;
	this->view_port.MaxDepth = 1.0f;
	this->view_port.TopLeftX = 0.0f;
	this->view_port.TopLeftY = 0.0f;
	this->deviceContext->RSSetViewports(1, &this->view_port);
}

bool RenderEngine::setupDepthStencilBuffer()
{
	//depth descriptions
	D3D11_TEXTURE2D_DESC depth_texture_desc;
	D3D11_DEPTH_STENCIL_VIEW_DESC dsv_desc;
	D3D11_DEPTH_STENCIL_DESC ds_desc;
	HRESULT hResult;



	//stencil operating of frontfacing
	//https://msdn.microsoft.com/en-us/library/windows/desktop/ff476110(v=vs.85).aspx << D3D11_DEPTH_STENCIL_DESC
	ZeroMemory(&ds_desc, sizeof(ds_desc));

	ds_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	ds_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	ds_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	ds_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//Stencil operation if backfacing
	ds_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	ds_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	ds_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	ds_desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//depthTestParameters
	ds_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	ds_desc.DepthFunc = D3D11_COMPARISON_LESS;

	//StencilTestParameters
	ds_desc.StencilEnable = FALSE;
	ds_desc.DepthEnable = TRUE;
	ds_desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	ds_desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

	hResult = this->device->CreateDepthStencilState(&ds_desc, &this->DSState);
	if (FAILED(hResult))
	{
		return false;
	}

	//create depth texture description, same as the rtv one
	ZeroMemory(&depth_texture_desc, sizeof(depth_texture_desc));
	depth_texture_desc.Width = this->WIDTH;
	depth_texture_desc.Height = this->HEIGHT;
	depth_texture_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depth_texture_desc.Usage = D3D11_USAGE_DEFAULT;
	depth_texture_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depth_texture_desc.MipLevels = 1;
	depth_texture_desc.ArraySize = 1;
	depth_texture_desc.CPUAccessFlags = 0;
	depth_texture_desc.MiscFlags = 0;
	depth_texture_desc.SampleDesc.Count = 1;
	depth_texture_desc.SampleDesc.Quality = 0;

	hResult = this->device->CreateTexture2D(&depth_texture_desc, NULL, &this->depthStencil_texture);
	if (FAILED(hResult))
	{
		return false;
	}

	//Create view of depthStencil
	//https://msdn.microsoft.com/en-us/library/windows/desktop/ff476112(v=vs.85).aspx << D3D11_DEPTH_STENCIL_VIEW_DESC
	ZeroMemory(&dsv_desc, sizeof(dsv_desc));
	dsv_desc.Format = depth_texture_desc.Format;
	dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsv_desc.Texture2D.MipSlice = 0;
	dsv_desc.Flags = 0;

	hResult = this->device->CreateDepthStencilView(this->depthStencil_texture, &dsv_desc, &this->depthStencilView);
	if (FAILED(hResult))
	{
		return false;
	}

	//https://msdn.microsoft.com/en-us/library/windows/desktop/ff476507(v=vs.85).aspx << CreateDepthStencilView
	//https://msdn.microsoft.com/en-us/library/windows/desktop/ff476521(v=vs.85).aspx << CreateTexture2D

	return true;
}

bool RenderEngine::setupRasterizer()
{
	//creating a custom rasterizer state

	D3D11_RASTERIZER_DESC rast_desc;
	HRESULT hResult;

	//https://msdn.microsoft.com/en-us/library/windows/desktop/ff476198(v=vs.85).aspx << D3D11_RASTERIZER_DESC

	ZeroMemory(&rast_desc, sizeof(rast_desc));
	rast_desc.FillMode = D3D11_FILL_SOLID;
	rast_desc.AntialiasedLineEnable = FALSE;
	rast_desc.DepthBias = 0;
	rast_desc.DepthBiasClamp = 0.0f;
	rast_desc.DepthClipEnable = TRUE;
	rast_desc.FrontCounterClockwise = FALSE;
	rast_desc.MultisampleEnable = FALSE;
	rast_desc.ScissorEnable = FALSE;
	rast_desc.SlopeScaledDepthBias = 0.0f;

	if (this->useRastBackCull)
	{
		rast_desc.CullMode = D3D11_CULL_BACK;
	}
	else
	{
		rast_desc.CullMode = D3D11_CULL_NONE;
	}
	
	hResult = this->device->CreateRasterizerState(&rast_desc, &this->RSState);
	if (FAILED(hResult))
	{
		return false;
	}

	return true;
}

bool RenderEngine::createCBs()
{
	//krakens allmighty constant buffers touch with care
	//wvp CB for VS
	D3D11_BUFFER_DESC cb_desc;
	ZeroMemory(&cb_desc, sizeof(cb_desc));
	cb_desc.ByteWidth = sizeof(this->m_wvp);
	cb_desc.Usage = D3D11_USAGE_DYNAMIC;
	cb_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb_desc.MiscFlags = 0;
	cb_desc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(data));
	data.pSysMem = &m_wvp;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	HRESULT hResult = this->device->CreateBuffer(&cb_desc, &data, &this->cb_matrixes);
	if (FAILED(hResult))
	{
		return false;
	}
	//this->deviceContext->VSSetConstantBuffers(0, 1, &this->cb_matrixes);
	this->deviceContext->GSSetConstantBuffers(0, 1, &this->cb_matrixes);

	//light CB for PS
	ZeroMemory(&cb_desc, sizeof(cb_desc));
	
	cb_desc.ByteWidth = sizeof(lights.getLights());
	cb_desc.Usage = D3D11_USAGE_DYNAMIC;
	cb_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb_desc.MiscFlags = 0;
	cb_desc.StructureByteStride = 0;

	ZeroMemory(&data, sizeof(data));
	data.pSysMem = &this->lights.getLights();
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	hResult = this->device->CreateBuffer(&cb_desc, &data, &this->cb_lights);
	if (FAILED(hResult))
	{
		return false;
	}
	this->deviceContext->PSSetConstantBuffers(0, 1, &this->cb_lights);
	return true;
}

void RenderEngine::setupOMS()
{
	this->deviceContext->RSSetState(this->RSState);
	this->deviceContext->OMSetDepthStencilState(this->DSState, 1);

}

void RenderEngine::setMatrixes(XMMATRIX world)
{
	//Projection
	this->m_wvp.projection = XMMatrixPerspectiveFovLH(XM_PI * 0.45f, this->WIDTH / this->HEIGHT, this->nearZ, this->farZ);
	this->m_wvp.view = this->camera.getView();
	this->m_wvp.world = world;
	this->m_wvp.wvp = this->m_wvp.world * this->m_wvp.view * this->m_wvp.projection; // world*view*proj
	this->m_wvp.vp = this->m_wvp.view * this->m_wvp.projection;
}

void RenderEngine::update()
{
	//update camera from keyboard and mouse
	this->camera.getInput();
	this->camera.update();
}

void RenderEngine::updateMatrixes()
{
	//update with new view from camera object
	this->m_wvp.view = this->camera.getView();
	this->m_wvp.wvp = this->m_wvp.world * this->m_wvp.view * this->m_wvp.projection; // world*view*proj
	this->m_wvp.vp = this->m_wvp.view * this->m_wvp.projection;
}

void RenderEngine::updateShaders(int in_pass)
{
	//updates shaders based on current pass

	switch (in_pass)
	{
	case RenderEngine::Geometry_pass:
		this->deviceContext->GSSetShader(this->deferred_shading.getGeoShader(), nullptr, 0);
		this->deviceContext->VSSetShader(this->deferred_shading.getGeoVS(), nullptr, 0);
		this->deviceContext->PSSetShader(this->deferred_shading.getGeoPS(), nullptr, 0);
		break;
	case RenderEngine::Lightning_pass:
		this->deviceContext->GSSetShader(nullptr, nullptr, 0);
		this->deviceContext->VSSetShader(this->deferred_shading.getLightVS(), nullptr, 0);
		this->deviceContext->PSSetShader(this->deferred_shading.getLightPS(), nullptr, 0);
		break;
	}
}

void RenderEngine::updateBuffers(ID3D11Buffer * in_VertexBuffer, ID3D11Buffer * in_IndexBuffer, float size_of_vertex)
{
	//sets vertex/index buffers to current object

	UINT weirdoInt = (UINT)size_of_vertex;
	UINT offset = 0.0f;

	this->deviceContext->IASetVertexBuffers(0, 1, &in_VertexBuffer, &weirdoInt, &offset);
	this->deviceContext->IASetIndexBuffer(in_IndexBuffer, DXGI_FORMAT_R32_UINT, offset);

}

void RenderEngine::clearRT()
{
	//clear the rendertargets before drawing with black
	for (size_t i = 0; i < this->VIEW_COUNT; i++)
	{
		this->deviceContext->ClearRenderTargetView(this->RTViews[i], this->black);
	}
	this->deviceContext->ClearRenderTargetView(this->back_buffer_view, this->black);
	this->deviceContext->ClearDepthStencilView(this->depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0.0f);
}

void RenderEngine::mapCBs()
{
	//maps the subresources of a constant buffer (locks it) and allows for rebinding 
	//and Unmaps, Unlocks it for use
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	this->deviceContext->Map(this->cb_matrixes, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, &this->m_wvp, sizeof(m_wvp));
	this->deviceContext->Unmap(this->cb_matrixes, 0);
}

void RenderEngine::layoutTopology(int in_topology, int in_layout)
{
	//changes topology and layout depending on the object
	switch (in_topology)
	{
	case TriangleList:
	{
		this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}
		break;
	}

	switch (in_layout)
	{
	case PN:
	{
		//Pos, Normal
		this->deviceContext->IASetInputLayout(this->deferred_shading.getPNLayout());
		this->PNActive = true;
	}
		break;
	case Pos:
	{
		this->deviceContext->IASetInputLayout(this->deferred_shading.getPosLayout());
		this->PNActive = false;
	}
		break;
	}
}

void RenderEngine::setDrawCall(int nr_verticies)
{

	//clears the rendertargets
	this->clearRT();

	//****geometry pass****\\

	//set correct shaders
	this->updateShaders(RenderEngine::Geometry_pass);

	//set g-buffer textures as rendertargets
	this->deviceContext->OMSetRenderTargets(this->VIEW_COUNT, this->RTViews.data(), this->depthStencilView);

	//draw vertices
	this->deviceContext->DrawIndexed(nr_verticies, 0, 0);

	//****lightpass****\\

	//set backbuffer as new rendertarget
	this->deviceContext->OMSetRenderTargets(1, &this->back_buffer_view, this->depthStencilView);

	//Set correct shaders
	this->updateShaders(RenderEngine::Lightning_pass);

	//Update Shader resource with texture views
	this->deviceContext->PSSetShaderResources(0, this->VIEW_COUNT, this->SRViews.data());

	//draw vertices for fullscreen quad
	this->setQuad(); 
	
	//reset resourceviews, untie SRViews from the shader to be used as rendertargets next frame
	this->deviceContext->PSSetShaderResources(0, this->VIEW_COUNT, this->null);

	//Present backbuffer to screen
	this->swapChain->Present(0, 0);


}

void RenderEngine::setQuad()
{

	this->layoutTopology(this->quad.getTopology(), this->quad.getLayout());
	this->updateBuffers(this->quad.getVertexBuffer(), this->quad.getIndexBuffer(), this->quad.getSizeOfVertex());
	this->deviceContext->DrawIndexed(this->quad.getNrOfVertices(), 0, 0);
	
}

RenderEngine::RenderEngine(HWND handle, HINSTANCE hInstance, int WIDHT, int HEIGHT)
{
	this->swapChain = nullptr;
	this->device = nullptr;
	this->deviceContext = nullptr;

	this->WIDTH = WIDHT;
	this->HEIGHT = HEIGHT;

	//initiate vectors
	ID3D11Texture2D* t_entry = nullptr;
	ID3D11ShaderResourceView* srv_entry = nullptr;
	ID3D11RenderTargetView* rtv_entry = nullptr;
	for (size_t i = 0; i < this->VIEW_COUNT; i++)
	{
		this->RTTextures.push_back(t_entry);
		this->RTViews.push_back(rtv_entry);
		this->SRViews.push_back(srv_entry);
	}

	//start up camera
	this->camera.initDI(hInstance, handle);

	//start up the rest of the deferred engine
	this->initiateEngine(handle);

	//setup shaders used for deferred
	this->deferred_shading.setDevice(this->device);
	this->deferred_shading.createVertexShaders();
	this->deferred_shading.createPixelShaders();
	this->deferred_shading.createGeometryShader();

	this->deviceContext->GSSetShader(this->deferred_shading.getGeoShader(), nullptr, 0);

	//setup lightpass quad
	this->quad.setDevice(this->device);
	this->quad.createBuffers();
	this->createCBs();

	//set clearRT to black
	this->black[0] = 0.0f;
	this->black[1] = 0.0f;
	this->black[2] = 0.0f;
	this->black[3] = 1.0f;
}

RenderEngine::~RenderEngine()
{
	/*
	Empty all vectors/delete contents
	Release all com-objects

	*/
	this->swapChain->Release();
	this->device->Release();
	this->deviceContext->Release();
	this->back_buffer_texture->Release();
	this->back_buffer_view->Release();
	this->depthStencil_texture->Release();
	this->depthStencilView->Release();
	this->cb_lights->Release();
	this->cb_matrixes->Release();

	for (size_t i = 0; i < this->VIEW_COUNT; i++)
	{
		this->RTViews[i]->Release();
		this->SRViews[i]->Release();
		this->RTTextures[i]->Release();
	}

}

void RenderEngine::Draw(Terrain * in_terrain)
{
	this->updateMatrixes();
	this->mapCBs();
	this->layoutTopology(in_terrain->getTopology(), in_terrain->getLayout());
	this->updateBuffers(in_terrain->getVertexBuffer(), in_terrain->getIndexBuffer(), in_terrain->getSizeOfVertex());
	this->setDrawCall(in_terrain->getNrOfVertices());

}

void RenderEngine::Draw(Geometry * in_geometry)
{
	this->updateMatrixes();
	this->mapCBs();
	this->updateBuffers(in_geometry->getVertexBuffer(), in_geometry->getIndexBuffer(), in_geometry->getSizeOfVertex());
	this->layoutTopology(in_geometry->getTopology(), in_geometry->getLayout());
	this->setDrawCall(in_geometry->getNrOfVertices());
}

ID3D11Device * RenderEngine::getDevice()
{
	return this->device;
}
