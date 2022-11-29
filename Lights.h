#pragma once
#include <DirectXMath.h>

using namespace DirectX;


enum LightType
{
	DIRECTIONAL_LIGHT,
	POINT_LIGHT,
	SPOT_LIGHT,
};

struct Light
{
	/// <summary>A colour vector that describes the diffuse light colour (RGBA)</summary>
	XMFLOAT4 diffuse;
	/// <summary>A colour vector that describes the ambient light colour (RGBA)</summary>
	XMFLOAT4 ambient;
	/// <summary>A color vector that describes the specular light (and as such, specular highlight color)</summary>
	XMFLOAT4 specular;
	/// <summary>The type of light, directional, point, or spot</summary>
	LightType type;

	Light()
	{
		diffuse = XMFLOAT4();
		ambient = XMFLOAT4();
		specular = XMFLOAT4();
	}
	Light(XMFLOAT4 diffuse, XMFLOAT4 ambient, XMFLOAT4 specular)
	{
		this->diffuse = diffuse;
		this->ambient = ambient;
		this->specular = specular;
	}
	
	virtual ~Light()
	{

	}
};

struct DirectionalLight : Light
{
	/// <summary>The light vector; this is a vector that points in the direction of the light source in the opposite direction of the incoming light rays</summary>
	XMFLOAT3 directionToLight;

	DirectionalLight() : Light()
	{
		directionToLight = XMFLOAT3();
		type = DIRECTIONAL_LIGHT;
	}
	DirectionalLight(XMFLOAT4 diffuse, XMFLOAT4 ambient, XMFLOAT4 specular, XMFLOAT3 directionToLight) : Light(diffuse, ambient, specular)
	{
		this->directionToLight = directionToLight;
		this->type = DIRECTIONAL_LIGHT;
	}
};

struct PointLight : Light
{
	/// <summary>The position of the light in 3D space</summary>
	XMFLOAT3 position;
	/// <summary>Stores the three attenuation constants in the format (a0, a1, a2) that control how light intensity falls off with distance</summary>
	XMFLOAT3 attenuation;
	/// <summary>A point whose distance from the light source is greater than the range will recieve no light from that source</summary>
	float range;

	PointLight() : Light()
	{
		position = XMFLOAT3();
		range = 0.0f;
		attenuation = XMFLOAT3();
		type = POINT_LIGHT;
	}
	PointLight(XMFLOAT4 diffuse, XMFLOAT4 ambient, XMFLOAT4 specular, XMFLOAT3 position, float range, XMFLOAT3 attenuation) : Light(diffuse, ambient, specular)
	{
		this->position = position;
		this->range = range;
		this->attenuation = attenuation;
		this->type = POINT_LIGHT;
	}
};

struct SpotLight : PointLight
{
	/// <summary>The direction vector; this is a vector that points in the direction of the light source in the same direction as the outgoing light rays</summary>
	XMFLOAT3 direction;
	/// <summary>The exponent used in the spotlight calculation to control the spotlight cone</summary>
	float spot;

	SpotLight() : PointLight()
	{
		direction = XMFLOAT3();
		spot = 0.0f;
		type = SPOT_LIGHT;
	}
	SpotLight(XMFLOAT4 diffuse, XMFLOAT4 ambient, XMFLOAT4 specular, XMFLOAT3 position, float range, XMFLOAT3 attenuation, XMFLOAT3 direction, float spot) : PointLight(diffuse, ambient, specular, position, range, attenuation)
	{
		this->direction = direction;
		this->spot = spot;
		this->type = SPOT_LIGHT;
	}
};