#ifndef DEFERREDSHADERS_H
#define DEFERREDSHADERS_H

#include"IncludeDX11.h"
#include"Settings.h"

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
	ID3D11VertexShader * geometry_vertex_shader;
	ID3D11VertexShader * light_vertex_shader;
	ID3D11PixelShader * geometry_pixel_shader;
	ID3D11PixelShader * light_pixel_shader;
	ID3D11GeometryShader * geometry_Shader;
	
	//inputLayouts
	ID3D11InputLayout* inp_PN_layout;
	ID3D11InputLayout* inp_Pos_layout;

	float ptn_size;
	float pn_size;
	float pc_size;

private:
	void compileGeometryPassShader(type in_type);
	void compileLightPassShader(type in_type);
	void createInputLayout(int in_type);
	
	void compileGeometryShader();
	
public:
	DeferredShaders();
	~DeferredShaders();

	ID3D11VertexShader* getGeoVS();
	ID3D11PixelShader* getGeoPS() const;
	ID3D11VertexShader* getLightVS() const;
	ID3D11PixelShader* getLightPS() const;
	ID3D11GeometryShader * getGeoShader() const;
	ID3D11InputLayout* getPNLayout() const;
	ID3D11InputLayout* getPosLayout() const;
	
	
	float getPTNSize() const;
	float getPNSize() const;
	float getPCSize() const;

	void createGeometryShader();
	void createVertexShaders();
	void createPixelShaders();
	void setDevice(ID3D11Device * in_device);
};
#endif // !DEFERREDSHADERS_H