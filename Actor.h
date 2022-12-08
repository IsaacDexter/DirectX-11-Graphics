#pragma once
#include <DirectXMath.h>
#include <d3d11_1.h>
#include <vector>

#include "Loading.h"

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

	/// <summary>ID3D11Buffer* pointer to vertex buffer</summary>
	ID3D11Buffer* m_vertexBuffer;
	/// <summary>ID3D11Buffer* pointer to index buffer</summary>
	ID3D11Buffer* m_indexBuffer;

	/// <summary>vertices, a vector SimpleVertex, which contain the local position and the normal of each vertex</summary>
	std::vector<SimpleVertex> m_vertices;
	/// <summary>indices, a vector of words containing the indices</summary>
	std::vector<WORD> m_indices;
	int m_indexCount;

	XMMATRIX m_position;
	XMMATRIX m_rotation;
	XMMATRIX m_scale;

	/// <summary>The objects model data</summary>
	Mesh* m_mesh;
	/// <summary>The Objects texture</summary>
	Texture* m_diffuseMap;
	/// <summary>The objects specular map. Leave blank to use the material's specular instead</summary>
	Texture* m_specularMap;
	/// <summary>The objects specular, ambient and diffuse</summary>
	Material* m_material;

public:
	Actor(Mesh* mesh, Material* material, Texture* diffuseMap, Texture* specularMap);
	~Actor();

	#pragma region Translation

	/// <param name="translation">Translation vector (xyz) to translate the object by</param>
	void Translate(XMFLOAT3 translation);
	/// <param name="translation">The Translation Matrix to translate the object by</param>
	void Translate(XMMATRIX translation);

	/// <param name="newPosition">The objects new (x, y, z) position</param>
	void SetPosition(XMFLOAT3 newPosition);
	/// <param name="newPosition">The translation matrix to set position to</param>
	void SetPosition(XMMATRIX newPosition);

	/// <param name="rotation"><para>X: Rotation around X (pitch)</para><para>Y: Rotation around Y (yaw)</para><para>Z: Rotation around Z (roll)</para></param>
	void Rotate(XMFLOAT3 rotation);
	/// <param name="rotation">The rotation matrix to rotate by</param>
	void Rotate(XMMATRIX rotation);

	/// <param name="newRotation"><para>X: New Rotation around X (pitch)</para><para>Y: New Rotation around Y (yaw)</para><para>Z: New Rotation around Z (roll)</para></param>
	void SetRotation(XMFLOAT3 newRotation);
	/// <param name="newRotation">The rotation matrix to set rotation to</param>
	void SetRotation(XMMATRIX newRotation);

	/// <param name="scale">The (xyz) to scale the object by</param>
	void Scale(XMFLOAT3 scale);
	/// <param name="scale">The scaling matrix to scale the object by</param>
	void Scale(XMMATRIX scale);

	/// <param name="newScale">The objects new (xyz) scale</param>
	void SetScale(XMFLOAT3 newScale);
	/// <param name="newScale">The objects new scaling matrix</param>
	void SetScale(XMMATRIX newScale);

	/// <param name="translation">Translation vector (x, y, z) to translate the object by</param>
	/// <param name="rotation">Rotation vector (pitch/around x, yaw/around y, roll/around z) to rotate the object by</param>
	/// <param name="scale">Scaling vector (x, y, z) to scale the object by</param>
	void Transform(XMFLOAT3 translation, XMFLOAT3 rotation, XMFLOAT3 scale);

	/// <param name="newPosition">The objects new position (x, y, z)</param>
	/// <param name="newRotation">The objects new rotation (pitch/around x, yaw/around y, roll/around z)</param>
	/// <param name="newScale">The objects new scale (x, y, z)</param>
	void SetTransform(XMFLOAT3 newPosition, XMFLOAT3 newRotation, XMFLOAT3 newScale);

	#pragma endregion

	void Update();
	void Draw(ID3D11DeviceContext* immediateContext, ID3D11Buffer* constantBuffer, ConstantBuffer cb);
};
