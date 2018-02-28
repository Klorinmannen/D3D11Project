#include "RenderEngine.h"

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

	for (size_t i = 0; i < this->RTV_VIEW_COUNT; i++)
	{
		this->RTViews.push_back(rtv_entry);

	}
	for (size_t i = 0; i < this->SRV_VIEW_COUNT; i++)
	{
		this->SRViews.push_back(srv_entry);
	}
	//start up camera
	this->camera.initDI(hInstance, handle);

	//start up the rest of the deferred engine
	this->initiateEngine(handle);

	//setup shaders used for deferred
	this->deferred_shading.setDevice(this->device);
	this->deferred_shading.compileVertexShaders();
	this->deferred_shading.compileGeometryShaders();
	this->deferred_shading.compilePixelShaders();

	//objloader
	this->objLoader.setDevice(this->device, this->deviceContext);

	//shadowmap
	this->sMap.setDevice(this->device);
	this->sMap.setWidthHeight(this->WIDTH, this->HEIGHT);
	this->sMap.createShadowShaders();
	this->sMap.createShadowDepthView();
	this->sMap.shadowMapSampler();

	//add shadowMap from light
	this->SRViews.push_back(this->sMap.getShadowSRV());
	//add depthMap from camera
	this->SRViews.push_back(this->depthSRV);

	//setup lightpass quad
	this->quad.setDevice(this->device);
	this->quad.createBuffers();
	this->createCBs();

	//set clearRT to black
	this->black[0] = 0.0f;
	this->black[1] = 0.0f;
	this->black[2] = 0.0f;
	this->black[3] = 1.0f;

	this->setMatrixes();
	this->setShadowStuff();

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
	this->back_buffer_view->Release();
	//this->depthStencil_texture->Release();
	this->depthStencilView->Release();
	this->cb_lights->Release();
	this->cb_matrixes->Release();
	this->RSState->Release();
	this->DSState->Release();
	this->depthSRV->Release();
	
	for (size_t i = 0; i < this->RTV_VIEW_COUNT; i++)
	{
		this->RTViews[i]->Release();
	}
	for (int i = 0; i < this->SRV_VIEW_COUNT; i++)
	{
		this->SRViews[i]->Release();
	}
}

void RenderEngine::loaderTest()
{
	//TESTTEST

	std::vector<XMFLOAT3> * vec1 = nullptr;
	std::vector<XMFLOAT3> * vec2 = nullptr;
	std::vector<XMFLOAT3> * vec3 =  nullptr;
	ID3D11ShaderResourceView* srv = nullptr;
	std::string temp_string = "box.OBJ";
	std::wstring tem_string = L"boxP.png";
}

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
	ID3D11Texture2D * back_buffer_texture;

	this->swapChain->GetBuffer(0, __uuidof(back_buffer_texture), reinterpret_cast<void**>(&back_buffer_texture));
	hresult = this->device->CreateRenderTargetView(back_buffer_texture, NULL, &this->back_buffer_view);
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

	//release com object
	back_buffer_texture->Release();

	return true;
}

bool RenderEngine::setupRTVs()
{
	//render&shader views
	D3D11_TEXTURE2D_DESC texture_desc;
	D3D11_RENDER_TARGET_VIEW_DESC rtv_desc;
	D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
	std::vector<ID3D11Texture2D*> RTTextures;

	ID3D11Texture2D* t_entry = nullptr; 

	//wnds msgs
	HRESULT hResult;

	//create textures
	ZeroMemory(&texture_desc, sizeof(texture_desc));
	texture_desc.Width = this->WIDTH;
	texture_desc.Height = this->HEIGHT;
	texture_desc.MipLevels = 1;
	texture_desc.ArraySize = 1; //one texture in the array
	texture_desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; 
	texture_desc.SampleDesc.Count = 1; 
	texture_desc.Usage = D3D11_USAGE_DEFAULT;
	texture_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE; //Use as render targer and shader resource
	texture_desc.CPUAccessFlags = 0;
	texture_desc.MiscFlags = 0;

	//create all necessary textures (normals, position, texture color)
	for (size_t i = 0; i < this->RTV_VIEW_COUNT; i++)
	{
		hResult = this->device->CreateTexture2D(&texture_desc, NULL, &t_entry);
		if (FAILED(hResult))
		{
			return false;
		}
		RTTextures.push_back(t_entry);
	}

	//create render target views
	
	ZeroMemory(&rtv_desc, sizeof(rtv_desc));
	rtv_desc.Format = texture_desc.Format;
	rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtv_desc.Texture2D.MipSlice = 0; 

	for (size_t i = 0; i < this->RTV_VIEW_COUNT; i++)
	{
		hResult = this->device->CreateRenderTargetView(RTTextures[i], &rtv_desc, &this->RTViews[i]);
		if (FAILED(hResult))
		{
			return false;
		}
	}

	//create shader resource views
	ZeroMemory(&srv_desc, sizeof(srv_desc));
	srv_desc.Format = texture_desc.Format;
	srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srv_desc.Texture2D.MipLevels = 1; 

	for (size_t i = 0; i < this->RTV_VIEW_COUNT; i++)
	{
		hResult = this->device->CreateShaderResourceView(RTTextures[i], &srv_desc, &this->SRViews[i]);
		if (FAILED(hResult))
		{
			return false;
		}
	}

	//release com objects
	for (int i = 0; i < this->RTV_VIEW_COUNT; i++)
	{
		RTTextures[i]->Release();
	}
	return true;
}

void RenderEngine::setupVP()
{

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

	ID3D11Texture2D * depthStencil_texture;
	HRESULT hResult;

	//stencil operating of frontfacing
	D3D11_DEPTH_STENCIL_DESC ds_desc;
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
	D3D11_TEXTURE2D_DESC depth_texture_desc;
	ZeroMemory(&depth_texture_desc, sizeof(depth_texture_desc));
	depth_texture_desc.Width = this->WIDTH;
	depth_texture_desc.Height = this->HEIGHT;
	depth_texture_desc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	depth_texture_desc.Usage = D3D11_USAGE_DEFAULT;
	depth_texture_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE; 
	depth_texture_desc.MipLevels = 1;
	depth_texture_desc.ArraySize = 1;
	depth_texture_desc.CPUAccessFlags = 0;
	depth_texture_desc.MiscFlags = 0;
	depth_texture_desc.SampleDesc.Count = 1;
	depth_texture_desc.SampleDesc.Quality = 0;

	hResult = this->device->CreateTexture2D(&depth_texture_desc, NULL, &depthStencil_texture);
	if (FAILED(hResult))
	{
		return false;
	}

	//Create view of depthStencil
	D3D11_DEPTH_STENCIL_VIEW_DESC dsv_desc;
	ZeroMemory(&dsv_desc, sizeof(dsv_desc));
	dsv_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsv_desc.Texture2D.MipSlice = 0;
	dsv_desc.Flags = 0;

	hResult = this->device->CreateDepthStencilView(depthStencil_texture, &dsv_desc, &this->depthStencilView);
	if (FAILED(hResult))
	{
		return false;
	}

	//create shadow SRV
	D3D11_SHADER_RESOURCE_VIEW_DESC dscSRV;
	ZeroMemory(&dscSRV, sizeof(dscSRV));
	dscSRV.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	dscSRV.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	dscSRV.Texture2D.MipLevels = 1;
	hResult = this->device->CreateShaderResourceView(depthStencil_texture, &dscSRV, &this->depthSRV);
	if (FAILED(hResult))
	{
		exit(-1);
	}

	depthStencil_texture->Release();

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
	rast_desc.DepthBias = 16;
	rast_desc.DepthBiasClamp = 0.0f;
	rast_desc.SlopeScaledDepthBias = 1.0f;
	rast_desc.DepthClipEnable = TRUE;
	rast_desc.FrontCounterClockwise = FALSE;
	rast_desc.MultisampleEnable = FALSE;
	rast_desc.ScissorEnable = FALSE;


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
	return true;
}

void RenderEngine::setupOMS()
{
	this->deviceContext->RSSetState(this->RSState);
	this->deviceContext->OMSetDepthStencilState(this->DSState, 1);

}

ObjLoader * RenderEngine::getLoader()
{
	return &this->objLoader;
}

void RenderEngine::setMatrixes()
{
	//Projection
	this->m_wvp.projection = XMMatrixPerspectiveFovLH(XM_PI * 0.45f, this->WIDTH / this->HEIGHT, this->nearZ, this->farZ);
	this->m_wvp.view = this->camera.getView();
	this->m_wvp.world = XMMatrixIdentity();
	this->m_wvp.wvp = this->m_wvp.world * this->m_wvp.view * this->m_wvp.projection; // world*view*proj

}

void RenderEngine::setShadowStuff()
{
	float farZz = 10000;
	//imagine a sphere incapsulating the scene 0, 0, 0 at centre and a radius of half farZ
	this->bSphere.centre = XMFLOAT3(6000.0f, 0.0f, -2000.0f);
	this->bSphere.radius = farZz;
	XMFLOAT4 temp = this->lights.getLights().direction;
	this->lightDir = XMLoadFloat4(&temp);

	//diameter * direction of light yields its position inside this imaginary sphere
	this->lightPos = 2.0f* this->bSphere.radius * this->lightDir;

	//store position into light object
	XMStoreFloat4(&temp, this->lightPos);
	this->lights.setLightPos(temp);

	//view matrix focal "focus" point is the centre of the sphere
	this->target = XMLoadFloat3(&bSphere.centre);

	//define up-vector0
	this->upVec = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	//create view matrix
	XMMATRIX view = XMMatrixLookAtLH(this->lightPos, this->target, this->upVec);

	//convert target into light view space
	XMFLOAT3 SphereCenterLS;
	XMStoreFloat3(&SphereCenterLS, XMVector3TransformCoord(this->target, view));

	//define values for the orthographic frustum in light view space
	float left = SphereCenterLS.x - farZ;
	float right = SphereCenterLS.x + farZ;
	float top = SphereCenterLS.x + farZ;
	float bottom = SphereCenterLS.x - farZ;
	float nearP = SphereCenterLS.x - farZ;
	float farP = SphereCenterLS.x + farZ;

	XMMATRIX orthoProjection = XMMatrixOrthographicOffCenterLH(left, right, bottom, top, this->nearZ, this->farZ);

	//calculate vpLight and bind to cb
	XMMATRIX viewProjection = view * orthoProjection;
	this->m_wvp.vpLight = viewProjection;
}

void RenderEngine::update()
{
	//update camera from keyboard and mouse
	this->camera.getInput();
	this->camera.update();
	this->lights.setCamPos(this->camera.getPosition());

}

void RenderEngine::updateMatrixes( const XMMATRIX &in_world)
{
	//update with new view from camera and new world
	this->m_wvp.world = in_world;
	this->m_wvp.view = this->camera.getView();
	this->m_wvp.wvp = this->m_wvp.world * this->m_wvp.view * this->m_wvp.projection; // world*view*proj
	this->m_wvp.inverseWorld = XMMatrixInverse(nullptr, this->m_wvp.world);
}

void RenderEngine::updateShaders(ID3D11VertexShader * in_vertexS, ID3D11GeometryShader * in_geometryS, ID3D11PixelShader * in_pixelS)
{
	//updates shaders
	this->deviceContext->VSSetShader(in_vertexS, nullptr, 0);
	this->deviceContext->GSSetShader(in_geometryS, nullptr, 0);
	this->deviceContext->PSSetShader(in_pixelS, nullptr, 0);
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
	for (size_t i = 0; i < this->RTV_VIEW_COUNT; i++)
	{
		this->deviceContext->ClearRenderTargetView(this->RTViews[i], this->black);
	}
	this->deviceContext->ClearRenderTargetView(this->back_buffer_view, this->black);
	this->deviceContext->ClearDepthStencilView(this->depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0.0f);
	this->deviceContext->ClearDepthStencilView(this->sMap.getDepthShadowView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0.0f);
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

	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	this->deviceContext->Map(this->cb_lights, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, &this->lights.getLights(), sizeof(this->lights.getLights()));
	this->deviceContext->Unmap(this->cb_lights, 0);
}

void RenderEngine::layoutTopology(int in_topology, int in_layout)
{
	//changes topology and layout depending on the object
	switch (in_topology)
	{
	case topology::TriangleList:
	{
		this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}
		break;
	case topology::TriangleStrip:
	{
		this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	}
		break;
	case topology::LineStrip:
	{
		this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
	}
		break;
	case topology::LineList:
	{
		this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	}
		break;
	case topology::TriangleStripAdj:
		{
			this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ);
		}
		break;
	}

	switch (in_layout)
	{
	case layout::PT:
	{
		//Pos, Normal
		this->deviceContext->IASetInputLayout(this->deferred_shading.getPTLayout());
		this->debugFlag = layout::PT;
		break;
	}

	case layout::Pos:
	{
		this->deviceContext->IASetInputLayout(this->deferred_shading.getPosLayout());
		this->debugFlag = layout::Pos;
		break;
	}

	case layout::PTN:
	{
		this->deviceContext->IASetInputLayout(this->deferred_shading.getPTNLayout());
		this->debugFlag = layout::PTN;
		break;
	}

	case layout::sMap:
	{
		this->deviceContext->IASetInputLayout(this->sMap.getInputLayout());
		break;
	}
	case layout::Cat:
	{
		this->deviceContext->IASetInputLayout(this->deferred_shading.getCatPTLayout());
		break;
	}
	}
}

void RenderEngine::setQuad()
{

	this->layoutTopology(this->quad.getTopology(), this->quad.getLayout());
	this->updateBuffers(this->quad.getVertexBuffer(), this->quad.getIndexBuffer(), this->quad.getSizeOfVertex());
	this->deviceContext->DrawIndexed(this->quad.getNrOfVertices(), 0, 0);
	
}

void RenderEngine::Draw(Terrain * in_terrain)
{
	this->updateMatrixes(in_terrain->getWorldMatrix());
	this->mapCBs();
	this->updateBuffers(in_terrain->getVertexBuffer(), in_terrain->getIndexBuffer(), in_terrain->getSizeOfVertex());
	
	//shadow pass
	this->deviceContext->VSSetConstantBuffers(0, 1, &this->cb_matrixes);
	this->updateShaders(this->sMap.getVertexShader(), nullptr, this->sMap.getPixelShader());
	this->layoutTopology(in_terrain->getTopology(), this->sMap.getLayout());
	this->shadowPass(in_terrain->getNrOfVertices(), drawType::Indexed);

	//set correct shaders
	this->layoutTopology(in_terrain->getTopology(), in_terrain->getLayout());
	this->updateShaders(this->deferred_shading.getGeometryPassVS(), this->deferred_shading.getGeoShader(), this->deferred_shading.getGeometryTerrainPS());
	this->deviceContext->VSSetConstantBuffers(0, 1, &this->nullBuffer);

	//set texture to sample from
	this->deviceContext->PSSetShaderResources(0, this->sampTerrainTextures.size(), this->sampTerrainTextures.data());

	//setConstantBuffers
	this->deviceContext->GSSetConstantBuffers(0, 1, &this->cb_matrixes);

	//no clue why this is has to be BUT IT HAS TO!
	this->tempState = in_terrain->getSamplerState();
	this->deviceContext->PSSetSamplers(0, 1, &this->tempState);

	//draw first pass
	this->geometryPass(in_terrain->getNrOfVertices(), drawType::Indexed);
	//reset
	this->deviceContext->PSSetShaderResources(0, this->sampTerrainTextures.size(), this->null2);
	this->deviceContext->GSSetConstantBuffers(0, 1, &this->nullBuffer);
	this->tempState = nullptr;
	this->deviceContext->PSSetSamplers(0, 1, &this->tempState);
}

void RenderEngine::Draw(Box * in_box)
{
	this->updateMatrixes(in_box->getWorldMatrix());
	this->mapCBs();
	this->updateBuffers(in_box->getVertexBuffer(), in_box->getIndexBuffer(), in_box->getSizeOfVertex());


	//shadow pass
	this->deviceContext->VSSetConstantBuffers(0, 1, &this->cb_matrixes);
	this->updateShaders(this->sMap.getVertexShader(), nullptr, this->sMap.getPixelShader());
	this->layoutTopology(in_box->getTopology(), this->sMap.getLayout());
	this->shadowPass(in_box->getNrOfVertices(), drawType::NonIndexed);

	//set correct shaders
	this->layoutTopology(in_box->getTopology(), in_box->getLayout());
	this->updateShaders(this->deferred_shading.getGeometryPassVS(), this->deferred_shading.getGeoShader(), this->deferred_shading.getGeometryBoxPS());
	this->deviceContext->VSSetConstantBuffers(0, 1, &this->nullBuffer);

	//set texture to sample from
	this->deviceContext->PSSetShaderResources(0, this->sampBoxTexture.size(), this->sampBoxTexture.data());
	
	//setConstantBuffers
	this->deviceContext->GSSetConstantBuffers(0, 1, &this->cb_matrixes);

	//draw first pass
	this->geometryPass(in_box->getNrOfVertices(), drawType::NonIndexed);
	//reset
	this->deviceContext->PSSetShaderResources(0, 1, this->null1);
	this->deviceContext->GSSetConstantBuffers(0, 1, &this->nullBuffer);
}

void RenderEngine::Draw(Plane * in_plane)
{
	this->updateMatrixes(in_plane->getWorldMatrix());
	this->mapCBs();
	this->updateBuffers(in_plane->getVertexBuffer(), in_plane->getIndexBuffer(), in_plane->getSizeOfVertex());
	
	//shadow pass
	this->deviceContext->VSSetConstantBuffers(0, 1, &this->cb_matrixes);
	this->updateShaders(this->sMap.getVertexShader(), nullptr, this->sMap.getPixelShader());
	this->layoutTopology(in_plane->getTopology(), this->sMap.getLayout());
	this->shadowPass(in_plane->getNrOfVertices(), drawType::Indexed);


	//set correct shaders
	this->layoutTopology(in_plane->getTopology(), in_plane->getLayout());
	this->updateShaders(this->deferred_shading.getGeometryPlaneVS(), this->deferred_shading.getGeomtryPlaneGS(), this->deferred_shading.getGeoemtryPlanePS());
	this->deviceContext->VSSetConstantBuffers(0, 1, &this->nullBuffer);

	//set texture to sample from 1 = stone
	this->deviceContext->PSSetShaderResources(0, 1, &this->sampTerrainTextures[1]);
	this->tempState = in_plane->getSamplerState();
	this->deviceContext->PSSetSamplers(0, 1, &this->tempState);

	//setConstantBuffers
	this->deviceContext->GSSetConstantBuffers(0, 1, &this->cb_matrixes);

	//draw first pass
	this->geometryPass(in_plane->getNrOfVertices(), drawType::Indexed);
	
	//reset
	this->deviceContext->GSSetConstantBuffers(0, 1, &this->nullBuffer);
	this->deviceContext->PSSetShaderResources(0, 1, this->null1);
	this->tempState = nullptr;
	this->deviceContext->PSSetSamplers(0, 1, &this->tempState);
}

void RenderEngine::Draw(Catnip * in_cat)
{
	this->updateMatrixes(in_cat->getWorldMatrix());
	this->mapCBs();
	this->updateBuffers(in_cat->getVertexBuffer(), in_cat->getIndexBuffer(), in_cat->getSizeOfVertex());


	//shadow pass
	this->deviceContext->VSSetConstantBuffers(0, 1, &this->cb_matrixes);
	this->updateShaders(this->sMap.getVertexShader(), nullptr, this->sMap.getPixelShader());
	this->layoutTopology(in_cat->getTopology(), this->sMap.getLayout());
	this->shadowPass(in_cat->getNrOfVertices(), drawType::Indexed);

	//set correct shaders
	this->layoutTopology(in_cat->getTopology(), in_cat->getLayout());
	this->updateShaders(this->deferred_shading.getGeometryPlaneVS(), this->deferred_shading.getGeomtryPlaneGS(), this->deferred_shading.getGeometryCatPS());
	this->deviceContext->VSSetConstantBuffers(0, 1, &this->nullBuffer);

	//set texture to sample from
	ID3D11ShaderResourceView * temp = in_cat->getCatSRV();
	this->deviceContext->PSSetShaderResources(0, 1, &temp);

	//setConstantBuffers
	this->deviceContext->GSSetConstantBuffers(0, 1, &this->cb_matrixes);

	//draw first pass
	this->geometryPass(in_cat->getNrOfVertices(), drawType::Indexed);



	//draw postprocess pass
	//update shaders

	//something is wrong and nothing happens.. i messed up. ***SOLVED*** SING SING SING
	//oh how do you do young willy McBride do you mind if i set here down by your graveside and rest for awhile in the warm summer sun, ive been walking all day and im nearly done.

	//remove cbs not used for post process
	this->deviceContext->GSSetConstantBuffers(0, 1, &this->nullBuffer);

	//bind new cbs
	this->deviceContext->VSSetConstantBuffers(0, 1, &this->cb_matrixes);
	this->layoutTopology(topology::TriangleList, layout::Cat);
	this->updateShaders(this->deferred_shading.getGeometryCatPostVS(), nullptr, this->deferred_shading.getGeometryCatPostPS()); 
	this->DrawPostProcess(in_cat->getNrOfVertices(), drawType::Indexed);

	//reset
	this->deviceContext->PSSetShaderResources(0, 1, this->null1);
	this->deviceContext->VSSetConstantBuffers(0, 1, &this->nullBuffer);



}

void RenderEngine::Draw(Drawable * in_object)
{
	Terrain* t_ptr = nullptr;
	Box* b_ptr = nullptr;
	Plane* p_ptr = nullptr;	
	Catnip* c_ptr = nullptr;

	c_ptr = dynamic_cast<Catnip*>(in_object);
	t_ptr = dynamic_cast<Terrain*>(in_object);
	b_ptr = dynamic_cast<Box*>(in_object);
	p_ptr = dynamic_cast<Plane*>(in_object);

	if (t_ptr != nullptr)
	{
		this->Draw(t_ptr);
	}
	else if(b_ptr != nullptr)
	{
		this->Draw(b_ptr);
	}
	else if (p_ptr != nullptr)
	{
		this->Draw(p_ptr);
	}
	else if (c_ptr != nullptr)
	{
		this->Draw(c_ptr);
	}
}

ID3D11Device * RenderEngine::getDevice()
{
	return this->device;
}

void RenderEngine::addBoxSRV(ID3D11ShaderResourceView * in_srv)
{
	this->sampBoxTexture.push_back(in_srv);
}

void RenderEngine::addTerrainSRV(ID3D11ShaderResourceView ** in_srv, int size)
{
	for (int i = 0; i < size; i++)
	{
		this->sampTerrainTextures.push_back(in_srv[i]);
	}
	//lpos texture view
}

void RenderEngine::clearRenderTargets()
{
	//clears the rendertargets
	this->clearRT();
}

void RenderEngine::presentSC()
{
	//Present backbuffer to screen
	this->swapChain->Present(0, 0);
}

void RenderEngine::lightPass()
{

	//****lightpass****\\

	this->deviceContext->PSSetConstantBuffers(0, 1, &this->cb_lights);
	ID3D11SamplerState * temp = this->sMap.getPointSample();
	this->deviceContext->PSSetSamplers(0, 1, &temp);
	//set backbuffer as new rendertarget
	this->deviceContext->OMSetRenderTargets(1, &this->back_buffer_view, 0);

	//Set correct shaders
	this->updateShaders(this->deferred_shading.getLightVS(), nullptr, this->deferred_shading.getLightPS());

	//Update Shader resource with texture views
	this->deviceContext->PSSetShaderResources(0, this->SRViews.size(), this->SRViews.data());

	//draw vertices for fullscreen quad
	this->setQuad();

	//reset resourceviews, untie SRViews from the shader to be used as rendertargets next frame
	this->deviceContext->PSSetShaderResources(0, this->SRViews.size(), this->null5);
	this->deviceContext->PSSetConstantBuffers(0, 1, &this->nullBuffer);
	this->deviceContext->PSSetSamplers(0, 1, &this->nullSampState);

	this->deviceContext->OMSetRenderTargets(0, 0, 0);
}

void RenderEngine::shadowPass(int nr_verticies, int drawType)
{
	this->deviceContext->OMSetRenderTargets(0, 0, this->sMap.getDepthShadowView());
	//draw vertices
	switch (drawType)
	{
	case drawType::Indexed:

		this->deviceContext->DrawIndexed(nr_verticies, 0, 0);
		break;
	case drawType::NonIndexed:

		this->deviceContext->Draw(nr_verticies, 0);
		break;
	default:
		exit(-1);
		break;
	}

	this->deviceContext->OMSetRenderTargets(0, 0, 0);
}

void RenderEngine::geometryPass(int nr_verticies, int drawType)
{

	//set g-buffer textures as rendertargets

	this->deviceContext->OMSetRenderTargets(this->RTViews.size(), this->RTViews.data(), this->depthStencilView);

	//draw vertices
	switch (drawType)
	{
	case drawType::Indexed:

		this->deviceContext->DrawIndexed(nr_verticies, 0, 0);
		break;
	case drawType::NonIndexed:

		this->deviceContext->Draw(nr_verticies, 0);
		break;
	default:
		exit(-1);
		break;
	}
	this->deviceContext->OMSetRenderTargets(0, 0, 0);
}

void RenderEngine::DrawPostProcess(int nr_verticies, int drawType)
{
	this->deviceContext->OMSetRenderTargets(1, &this->RTViews[2], nullptr);

	//draw vertices
	switch (drawType)
	{
	case drawType::Indexed:

		this->deviceContext->DrawIndexed(nr_verticies, 0, 0);
		break;
	case drawType::NonIndexed:

		this->deviceContext->Draw(nr_verticies, 0);
		break;
	default:
		exit(-1);
		break;
	}
	this->deviceContext->OMSetRenderTargets(0, 0, 0);
}

XMFLOAT4 RenderEngine::getCamPos()
{
	return this->camera.getPosition();
}


