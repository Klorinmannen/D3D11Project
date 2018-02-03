#pragma once
#include"IncludeDX11.h"

//shaders for deferred rendering technique

class DeferredShaders
{
public:
	enum type { Vertex_S, Pixel_S };

private:
	HRESULT hResult;

	ID3D11Device * device;
	ID3DBlob * shader_blob;
	ID3DBlob * error_blob;

	//shader handles
	ID3D11VertexShader * vertex_shader;
	ID3D11GeometryShader * geometry_shader;
	ID3D11PixelShader * pixel_shader;

	//inputLayouts
	ID3D11InputLayout* PTN_layout;
	ID3D11InputLayout* PN_layout;


private:
	void compileShader(type in_type);
	void createInputLayout();
	void createVertexShader();
	void createPixelShader();

public:
	DeferredShaders(ID3D11Device* &in_device);
	~DeferredShaders();

	const ID3D11VertexShader* getVS();
	const ID3D11PixelShader* getPS();
	const ID3D11InputLayout* getPTNLayout();
	const ID3D11InputLayout* getPNLayout();

};