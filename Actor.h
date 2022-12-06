#pragma once
#include <DirectXMath.h>
#include <d3d11_1.h>
#include <vector>

#include "OBJLoader.h"

#include "Materials.h"
#include "Vertices.h"
#include "Buffers.h"
#include "Normals.h"

using namespace DirectX;

/// <summary><para>Stores all the information about an object: <br/>
///  - indices, a vector of words containing the indices <br/>
///  - vertices, a vector SimpleVertex, which contain the local position and the normal of each vertex <br/>
///  - ID3D11Buffer* pointers to vertex, and index buffers <br/>
///  - An XMFLOAT4X4 4 by 4 matrix containing the world transforms of the object <br/>
/// </para></summary>
class Actor
{
protected:
	/// <summary>An XMFLOAT4X4 4 by 4 matrix containing the world transforms of the object</summary>
	XMFLOAT4X4 m_world;
	/// <summary>A pointer to the direct 3d Device, needed to initialise buffers</summary>
	ID3D11Device* m_pd3dDevice;

	/// <summary>ID3D11Buffer* pointer to vertex buffer</summary>
	ID3D11Buffer* m_vertexBuffer;
	/// <summary>ID3D11Buffer* pointer to index buffer</summary>
	ID3D11Buffer* m_indexBuffer;

	/// <summary>vertices, a vector SimpleVertex, which contain the local position and the normal of each vertex</summary>
	std::vector<SimpleVertex> m_vertices;
	/// <summary>indices, a vector of words containing the indices</summary>
	std::vector<WORD> m_indices;

	/// <summary>The objects model data</summary>
	MeshData m_mesh;

	/// <summary>The objects specular, ambient and diffuse</summary>
	Material m_material;
	
protected:
	virtual HRESULT InitRenderedObject();
	HRESULT InitVertexBuffer();
	HRESULT InitIndexBuffer();

public:
	Actor(ID3D11Device* _pd3dDevice);
	~Actor();

	void Update(XMMATRIX transform);
	void Draw(ID3D11DeviceContext* immediateContext, ID3D11Buffer* constantBuffer, ConstantBuffer cb);
};

class Cube : public Actor
{
public:
	Cube(ID3D11Device* _p3dDevice);
	HRESULT InitRenderedObject() override;
};

class Pyramid : public Actor
{
public:
	Pyramid(ID3D11Device* _p3dDevice);
	HRESULT InitRenderedObject() override;
};
