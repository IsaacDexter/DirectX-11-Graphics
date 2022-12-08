#pragma once
#include <DirectXMath.h>
#include <d3d11_1.h>
#include <vector>

#include "Loading.h"

#include "Materials.h"
#include "Vertices.h"
#include "Buffers.h"
#include "Normals.h"
#include "Camera.h"
#include "Actor.h"

class Level
{
private:
	ID3D11DeviceContext* m_immediateContext;
	ID3D11Buffer* m_constantBuffer;
	ID3D11Device* m_d3dDevice;

	XMFLOAT4X4				m_world;
	XMFLOAT4X4              m_view;

	Camera* m_camera;
	std::map<std::string, Texture*>* _textures;
	std::map<std::string, Mesh*>* _meshes;
	std::map<std::string, Material*>* _materials;
	std::map<std::string, Actor*>* _actors;
	std::map<std::string, DirectionalLight*>* _directionalLights;
	std::map<std::string, PointLight*>* _pointLights;
	std::map<std::string, SpotLight*>* _spotLights;
public:
	Level(char* path, ID3D11Device* d3dDevice, ID3D11DeviceContext* immediateContext, ID3D11Buffer* constantBuffer);
	~Level();
	void Update(float t);
	void Draw(ConstantBuffer* cb);
private:
	void Load(char* path);

	void InitObjects();
	void LoadTextures();
	void LoadMeshes();
	void LoadMaterials();
	void LoadLights();

	void UpdateActors();
	void DrawActors(ConstantBuffer* cb);

	/// <summary>Stores lights of the directional type from their respective maps into the constant buffer. Cleans up draw code a bit</summary>
	/// <param name="cb">A pointer to the constant buffer</param>
	void StoreDirectionalLights(ConstantBuffer* cb);
	/// <summary>Stores lights of the point type from their respective maps into the constant buffer. Cleans up draw code a bit</summary>
	/// <param name="cb">A pointer to the constant buffer</param>
	void StorePointLights(ConstantBuffer* cb);
	/// <summary>Stores lights of the spot type from their respective maps into the constant buffer. Cleans up draw code a bit</summary>
	/// <param name="cb">A pointer to the constant buffer</param>
	void StoreSpotLights(ConstantBuffer* cb);
};

