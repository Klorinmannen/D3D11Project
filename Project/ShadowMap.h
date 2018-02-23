#ifndef SHADOWMAP_H
#define SHADOWMAP_H
#include"IncludeDX11.h"
#include"Settings.h"

class ShadowMap
{
private:
	ID3D11Device * device;
	ID3D11DepthStencilView * shadowDepthView;
	ID3D11ShaderResourceView * shadowSRV;

	ID3DBlob* shaderBlob;
	ID3DBlob* errorBlob;
	ID3D11VertexShader* shadowShader_VS;
	ID3D11PixelShader* shadowShader_PS;
	ID3D11InputLayout* inp_Pos_layout;

	int width;
	int height;
	int layout;

public:
	ShadowMap();
	~ShadowMap();

	ID3D11DepthStencilView * getDepthShadowView();
	ID3D11ShaderResourceView * getShadowSRV();
	ID3D11InputLayout* getInputLayout();
	ID3D11VertexShader* getVertexShader();
	ID3D11PixelShader* getPixelShader();
	int getLayout();

	void setWidthHeight(int widht, int height);
	void setDevice(ID3D11Device * in_device);
	void createShadowShaders();
	void createShadowDepthView();
};
#endif // !SHADOWMAP_H
