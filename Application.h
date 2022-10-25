#pragma once

#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include "resource.h"
#include <vector>

using namespace DirectX;

struct SimpleVertex
{
    XMFLOAT3 Pos;
    XMFLOAT3 Normal;
};

struct SimpleVertex_Vector
{
	XMVECTOR Pos;
	XMVECTOR Normal;
};

struct ConstantBuffer
{
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProjection;
};

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
	ID3D11VertexShader*     _pVertexShader;
	ID3D11PixelShader*      _pPixelShader;
	/// <summary>A rasterizer state used to draw objects as wireframe.</summary>
	ID3D11RasterizerState*	_wireFrame;
	/// <summary>A rasterizer state used to draw in block, solid fill.</summary>
	ID3D11RasterizerState*	_solidFill;
	/// <summary>Used to track the current rasterizer state when switching between cube and wireframe.</summary>
	ID3D11RasterizerState*	_currentRasterizerState;
	ID3D11InputLayout*      _pVertexLayout;
	/// <summary>A buffer to store the indexed vertices of the cube object.</summary>
	ID3D11Buffer*           _pCubeVertexBuffer;
	/// <summary>A buffer to store the indexed vertices of the pyramid object.</summary>
	ID3D11Buffer*           _pPyramidVertexBuffer;
	/// <summary>A buffer to store the indices of the cube object.</summary>
	ID3D11Buffer*           _pCubeIndexBuffer;
	/// <summary>A buffer to store the indices of the pyramid object.</summary>
	ID3D11Buffer*           _pPyramidIndexBuffer;
	ID3D11Buffer*           _pConstantBuffer;
	/// <summary>Interface Used to store the depth/stencil view</summary>
	ID3D11DepthStencilView*	_depthStencilView;
	/// <summary>Interface Used to store the depth/stencil buffer</summary>
	ID3D11Texture2D*		_depthStencilBuffer;
	// World matrices to hold the transformations for the 2 objects.
	XMFLOAT4X4              _world, _world2;
	XMFLOAT4X4              _view;
	XMFLOAT4X4              _projection;
	/// <summary>The currently hardcoded vertices of the pyramid. Replace with a model loader eventually.</summary>
	std::vector<SimpleVertex>	_pyramidVertices;
	/// <summary>The currently hardcoded vertices of the cube. Replace with a model loader eventually.</summary>
	std::vector<SimpleVertex>	_cubeVertices;
	/// <summary>Avoiding hardcoded values. The number of indices in the pyramid (18)</summary>
	int						_pPyramidIndicesCount;
	/// <summary>Avoiding hardcoded values. The number of indices in the cube (36)</summary>
	int						_pCubeIndicesCount;	

private:
	HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
	HRESULT InitDevice();
	void Cleanup();
	HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
	HRESULT InitShadersAndInputLayout();
	HRESULT InitPyramidVertexBuffer();
	HRESULT InitCubeVertexBuffer();
	HRESULT InitPyramidIndexBuffer();
	HRESULT InitCubeIndexBuffer();
	void CalculateNormals(std::vector<SimpleVertex>* Vertices, WORD* Indices, int IndicesCount);

	UINT _WindowHeight;
	UINT _WindowWidth;

public:
	Application();
	~Application();

	HRESULT Initialise(HINSTANCE hInstance, int nCmdShow);

	void Update();
	void Draw();
};

