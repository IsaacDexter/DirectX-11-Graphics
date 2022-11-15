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

	//A colour vector that describes the diffuse light colour. (RGBA)
	XMFLOAT4 DiffLight;
	//A colour vector that describes the diffuse materials reflectiveness.
	XMFLOAT4 DiffMat;
	//The light vector; this is a vector that points in the direction of the light source in the opposite direction of the incoming light rays

	//A colour vector that describes the diffuse light colour. (RGBA)
	XMFLOAT4 AmbLight;
	//A colour vector that describes the diffuse materials reflectiveness.
	XMFLOAT4 AmbMat;

	XMFLOAT3 DirToLight;
};

struct Light
{
	XMFLOAT4 diffuse;
	XMFLOAT4 ambient;
	XMFLOAT3 directionToLight;
};

struct LightingMaterial
{
	XMFLOAT4 diffuse;
	XMFLOAT4 ambient;
};

/// <summary><para>Stores all the information about an object: <br/>
///  - indices, a vector of words containing the indices <br/>
///  - vertices, a vector SimpleVertex, which contain the local position and the normal of each vertex <br/>
///  - ID3D11Buffer* pointers to vertex, and index buffers <br/>
///  - An XMFLOAT4X4 4 by 4 matrix containing the world transforms of the object <br/>
/// </para></summary>
class RenderedObject
{

protected:
	/// <summary>ID3D11Buffer* pointer to vertex buffer</summary>
	ID3D11Buffer* m_vertexBuffer;
	/// <summary>ID3D11Buffer* pointer to index buffer</summary>
	ID3D11Buffer* m_indexBuffer;
	/// <summary>An XMFLOAT4X4 4 by 4 matrix containing the world transforms of the object</summary>
	XMFLOAT4X4 m_world;
	/// <summary>indices, a vector of words containing the indices</summary>
	std::vector<WORD> m_indices;
	/// <summary>A pointer to the direct 3d Device, needed to initialise buffers</summary>
	ID3D11Device* m_pd3dDevice;
	LightingMaterial m_material;
	/// <summary>vertices, a vector SimpleVertex, which contain the local position and the normal of each vertex</summary>
	std::vector<SimpleVertex> m_vertices;
protected:
	virtual HRESULT InitRenderedObject();
	HRESULT InitVertexBuffer();
	HRESULT InitIndexBuffer();

	void CalculateSmoothNormals(std::vector<SimpleVertex>* Vertices, std::vector<WORD>* Indices);
	void CalculateFlatNormals(std::vector<SimpleVertex>* Vertices, std::vector<WORD>* Indices);
	

public:
	RenderedObject(ID3D11Device* _pd3dDevice);
	~RenderedObject();

	void Update(XMMATRIX transform);
	void Draw(ID3D11DeviceContext* immediateContext, ID3D11Buffer* constantBuffer, ConstantBuffer cb);
};

class Pyramid: public RenderedObject
{
public:
	Pyramid(ID3D11Device* _pd3dDevice);
	~Pyramid();
	HRESULT InitRenderedObject() override;
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
	RenderedObject*			_cube;
	Pyramid*				_pyramid;
	Light*					_light;

private:
	HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
	HRESULT InitDevice();
	HRESULT InitObjects();
	void Cleanup();
	HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
	HRESULT InitShadersAndInputLayout();
	

	UINT _WindowHeight;
	UINT _WindowWidth;

public:
	Application();
	~Application();

	HRESULT Initialise(HINSTANCE hInstance, int nCmdShow);

	void Update();
	void Draw();
};

