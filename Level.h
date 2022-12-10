#pragma once
#include <fstream>
#include <DirectXMath.h>
#include <d3d11_1.h>
#include <vector>

#include "Loading.h"
#include "include/nlohmann/json.hpp"
#include "Keyboard.h"
#include "Mouse.h"

#include "Materials.h"
#include "Vertices.h"
#include "Buffers.h"
#include "Normals.h"
#include "Camera.h"
#include "Actor.h"

using json = nlohmann::json;

class Level
{
private:
	ID3D11DeviceContext* m_immediateContext;
	ID3D11Buffer* m_constantBuffer;
	ID3D11Device* m_d3dDevice;
	XMFLOAT2 m_windowSize;

	XMFLOAT4X4				m_world;

	Camera* m_camera;

	std::map<std::string, Camera*>* _cameras;
	std::map<std::string, Texture*>* _textures;
	std::map<std::string, Mesh*>* _meshes;
	std::map<std::string, Material*>* _materials;
	std::map<std::string, Actor*>* _actors;
	std::map<std::string, DirectionalLight*>* _directionalLights;
	std::map<std::string, PointLight*>* _pointLights;
	std::map<std::string, SpotLight*>* _spotLights;
public:
	Level(char* path, ID3D11Device* d3dDevice, ID3D11DeviceContext* immediateContext, ID3D11Buffer* constantBuffer, XMFLOAT2 windowSize);
	~Level();
	void Update(float t, Keyboard::State kb, Mouse::State);
	void Draw();
private:
	void Load(char* path);

	void LoadTexture(std::string name, std::string path);
	void LoadMesh(std::string name, std::string path);
	void LoadMaterial(std::string name, std::string path);
	void LoadMaterial(std::string name, XMFLOAT4 diffuse, XMFLOAT4 ambient, XMFLOAT4 specular, float specularFalloff);
	
	void LoadDirectionalLight(std::string name, XMFLOAT4 diffuse, XMFLOAT4 ambient, XMFLOAT4 specular, XMFLOAT3 direction);
	void LoadPointLight(std::string name, XMFLOAT4 diffuse, XMFLOAT4 ambient, XMFLOAT4 specular, XMFLOAT3 position, XMFLOAT3 attenuation, float range);
	void LoadSpotLight(std::string name, XMFLOAT4 diffuse, XMFLOAT4 ambient, XMFLOAT4 specular, XMFLOAT3 position, XMFLOAT3 attenuation, float range, XMFLOAT3 direction, float spot);
	
	void LoadActor(std::string name, std::string mesh, std::string material, std::string diffuseMap, std::string specularMap, XMFLOAT3 position, XMFLOAT3 rotation, XMFLOAT3 scale);
	void LoadCamera(std::string name, XMFLOAT4 eye, XMFLOAT4 at, XMFLOAT4 up, float windowWidth, float windowHeight, float nearDepth, float farDepth);

	void LoadTextures(json jFile);
	void LoadMeshes(json jFile);
	void LoadMaterials(json jFile);

	void LoadDirectionalLights(json jFile);
	void LoadPointLights(json jFile);
	void LoadSpotLights(json jFile);

	void LoadActors(json jFile);
	void LoadCameras(json jFile);

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

