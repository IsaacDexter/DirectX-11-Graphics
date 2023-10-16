#pragma once
#include <DirectXMath.h>
#include <d3d11_1.h>
#include <vector>

#include "Loading.h"

#include "Materials.h"
#include "Vertices.h"
#include "Buffers.h"
#include "Normals.h"

class Billboard
{
private:
	/// <summary>An XMFLOAT4X4 4 by 4 matrix containing the world transforms of the center of the base of the billboard</summary>
	XMFLOAT4X4 m_world;
	XMFLOAT3 m_right;
	XMFLOAT3 m_normal;

	SimpleVertex m_vertices[4];

	XMFLOAT3 m_position;
	XMFLOAT3 m_rotation;
	XMFLOAT2 m_size;

	ID3D11Buffer* m_indexBuffer;
	ID3D11Buffer* m_vertexBuffer;
	ID3D11Device* m_d3dDevice;

	D3D11_BUFFER_DESC m_vertexBufferDesc;

	/// <summary>The billboards texture</summary>
	Texture* m_diffuseMap;
	/// <summary>The billboards specular, ambient and diffuse</summary>
	Material* m_material;
public:
	Billboard(XMFLOAT3 position, XMFLOAT2 size, Texture* texture, Material* material, ID3D11Device* d3dDevice);
	void Update(XMFLOAT3 cameraPos);
	void Draw(ID3D11DeviceContext* immediateContext, ID3D11Buffer* constantBuffer, ConstantBuffer cb);
private:
	void SetPosition(XMFLOAT3 newPosition);
};

