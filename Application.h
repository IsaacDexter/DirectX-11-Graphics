#pragma once

#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include <vector>
#include <map>
#include "resource.h"

#include "Loading.h"

#include "Lights.h"
#include "Materials.h"
#include "Camera.h"
#include "Actor.h"


using namespace DirectX;

class Application
{
private:
	HINSTANCE               _hInst;
	HWND                    _hWnd;
	D3D_DRIVER_TYPE         _driverType;
	D3D_FEATURE_LEVEL       _featureLevel;
	ID3D11Device*           _pd3dDevice;
	ID3D11DeviceContext*    _pImmediateContext;
	IDXGISwapChain*         _pSwapChain;
	ID3D11RenderTargetView* _pRenderTargetView;
	/// <summary> Holds the texture sampler, passed across to texture shader for use</summary>
	ID3D11SamplerState*		_pSamplerLinear;
	ID3D11VertexShader*     _pVertexShader;
	ID3D11PixelShader*      _pPixelShader;
	/// <summary>A rasterizer state used to draw objects as wireframe.</summary>
	ID3D11RasterizerState*	_wireFrame;
	/// <summary>A rasterizer state used to draw in block, solid fill.</summary>
	ID3D11RasterizerState*	_solidFill;
	/// <summary>Used to track the current rasterizer state when switching between cube and wireframe.</summary>
	ID3D11RasterizerState*	_currentRasterizerState;
	ID3D11InputLayout*      _pVertexLayout;
	ID3D11Buffer*			_pVertexBuffer;
	ID3D11Buffer*			_pIndexBuffer;
	ID3D11Buffer*           _pConstantBuffer;
	/// <summary>Interface Used to store the depth/stencil view</summary>
	ID3D11DepthStencilView*	_depthStencilView;
	/// <summary>Interface Used to store the depth/stencil buffer</summary>
	ID3D11Texture2D*		_depthStencilBuffer;
	XMFLOAT4X4				_world;
	XMFLOAT4X4              _view;
	XMFLOAT4X4              _projection;


	Camera*	_camera;
	std::map<std::string, Texture*>*			_textures;
	std::map<std::string, Mesh*>*				_meshes;
	std::map<std::string, Material*>*			_materials;						
	std::map<std::string, Actor*>*				_actors;
	std::map<std::string, DirectionalLight*>*	_directionalLights;
	std::map<std::string, PointLight*>*			_pointLights;
	std::map<std::string, SpotLight*>*			_spotLights;
private:
	HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
	HRESULT InitDevice();
	HRESULT InitShadersAndInputLayout();

	void InitObjects();
	void LoadTextures();
	void LoadMeshes();
	void LoadMaterials();
	void LoadLights(); 

	void UpdateActors();
	void DrawActors(ID3D11DeviceContext* immediateContext, ID3D11Buffer* constantBuffer, ConstantBuffer cb);

	/// <summary>Stores lights of the directional type from their respective maps into the constant buffer. Cleans up draw code a bit</summary>
	/// <param name="cb">A pointer to the constant buffer</param>
	void StoreDirectionalLights(ConstantBuffer* cb);
	/// <summary>Stores lights of the point type from their respective maps into the constant buffer. Cleans up draw code a bit</summary>
	/// <param name="cb">A pointer to the constant buffer</param>
	void StorePointLights(ConstantBuffer* cb);
	/// <summary>Stores lights of the spot type from their respective maps into the constant buffer. Cleans up draw code a bit</summary>
	/// <param name="cb">A pointer to the constant buffer</param>
	void StoreSpotLights(ConstantBuffer* cb);

	void Cleanup();
	
	HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);

	UINT _WindowHeight;
	UINT _WindowWidth;

public:
	Application();
	~Application();

	HRESULT Initialise(HINSTANCE hInstance, int nCmdShow);

	void Update();
	void Draw();
};

