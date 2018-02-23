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

	ID3D11Device * device;
	ID3DBlob * shader_blob;
	ID3DBlob * error_blob;

	//shaders
	ID3D11VertexShader * GeometryPass_VS;

	ID3D11VertexShader * LightPass_VS;
	ID3D11PixelShader * LightPass_PS;

	ID3D11PixelShader * GeometryPassTerrain_PS;
	ID3D11PixelShader * GeometryPassBox_PS;

	ID3D11GeometryShader * geometry_Shader;
	
	ID3D11PixelShader * GeometryPassPlane_PS;
	ID3D11VertexShader * GeometryPassPlane_VS;
	ID3D11GeometryShader * GeometryPassPlane_GS;

	//inputLayouts
	ID3D11InputLayout* inp_PN_layout;
	ID3D11InputLayout* inp_Pos_layout;
	ID3D11InputLayout* inp_PTN_layout;
	ID3D11InputLayout* inp_PT_layout;
	float ptn_size;
	float pn_size;
	float pc_size;


public:
	DeferredShaders();
	~DeferredShaders();

	ID3D11VertexShader* getGeometryPassVS() const;
	ID3D11PixelShader* getGeometryTerrainPS() const;
	ID3D11PixelShader* getGeometryBoxPS() const;

	ID3D11PixelShader* getGeoemtryPlanePS() const;
	ID3D11VertexShader* getGeometryPlaneVS() const;
	ID3D11GeometryShader* getGeomtryPlaneGS() const;

	ID3D11VertexShader* getLightVS() const;
	ID3D11PixelShader* getLightPS() const;

	ID3D11GeometryShader * getGeoShader() const;

	ID3D11InputLayout* getPTLayout() const;
	ID3D11InputLayout* getPosLayout() const;
	ID3D11InputLayout* getPTNLayout() const;
	
	float getPTNSize() const;
	float getPNSize() const;
	float getPCSize() const;
	void setDevice(ID3D11Device * in_device);
	void compileVertexShaders();
	void compilePixelShaders();
	void compileGeometryShaders();
};
#endif // !DEFERREDSHADERS_H