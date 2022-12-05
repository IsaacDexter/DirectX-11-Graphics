#pragma once
#include <DirectXMath.h>

using namespace DirectX;


enum LightType
{
	DIRECTIONAL_LIGHT,
	POINT_LIGHT,
	SPOT_LIGHT,
};

/// <summary>Abstract parent class to the rest of the lights. could be used in a vector to cast to the individual lights</summary>
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
		diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
		ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
		specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
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
		directionToLight = XMFLOAT3(0.0f, 0.0f, 0.0f);
		type = DIRECTIONAL_LIGHT;
	}
	/// <summary>A light that shines all objects from a given direction, like the sun</summary>
	/// <param name="diffuse">A colour vector that describes the diffuse light colour (RGBA)</param>
	/// <param name="ambient">A colour vector that describes the ambient light colour (RGBA)</param>
	/// <param name="specular">A color vector that describes the specular light (and as such, specular highlight color)</param>
	/// <param name="directionToLight">The light vector; this is a vector that points in the direction of the light source in the opposite direction of the incoming light rays</param>
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
	/// <summary>A point whose distance from the light source is greater than the range will recieve no light from that source</summary>
	float range;
	/// <summary>Stores the three attenuation constants in the format (a0, a1, a2) that control how light intensity falls off with distance</summary>
	XMFLOAT3 attenuation;

	PointLight() : Light()
	{
		position = XMFLOAT3(0.0f, 0.0f, 0.0f);
		range = 0.0f;
		attenuation = XMFLOAT3(0.0f, 0.0f, 0.0f);
		type = POINT_LIGHT;
	}
	/// <summary>A light with a position in space that shines outwards in all directions upto a given range</summary>
	/// <param name="diffuse">A colour vector that describes the diffuse light colour (RGBA)</param>
	/// <param name="ambient">A colour vector that describes the ambient light colour (RGBA)</param>
	/// <param name="specular">A color vector that describes the specular light (and as such, specular highlight color)</param>
	/// <param name="position">The position of the light in 3D space</param>
	/// <param name="range">A point whose distance from the light source is greater than the range will recieve no light from that source</param>
	/// <param name="attenuation">Stores the three attenuation constants in the format (a0, a1, a2) that control how light intensity falls off with distance</param>
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
		direction = XMFLOAT3(0.0f, 0.0f, 0.0f);
		spot = 0.0f;
		type = SPOT_LIGHT;
	}
	/// <summary>A light with a position in space that shines a cone in a given direction with a given radius</summary>
	/// <param name="diffuse">A colour vector that describes the diffuse light colour (RGBA)</param>
	/// <param name="ambient">A colour vector that describes the ambient light colour (RGBA)</param>
	/// <param name="specular">A color vector that describes the specular light (and as such, specular highlight color)</param>
	/// <param name="position">The position of the light in 3D space</param>
	/// <param name="range">A point whose distance from the light source is greater than the range will recieve no light from that source</param>
	/// <param name="attenuation">Stores the three attenuation constants in the format (a0, a1, a2) that control how light intensity falls off with distance</param>
	/// <param name="direction">The direction vector; this is a vector that points in the direction of the light source in the same direction as the outgoing light rays</param>
	/// <param name="spot">The exponent used in the spotlight calculation to control the spotlight cone</param>
	SpotLight(XMFLOAT4 diffuse, XMFLOAT4 ambient, XMFLOAT4 specular, XMFLOAT3 position, float range, XMFLOAT3 attenuation, XMFLOAT3 direction, float spot) : PointLight(diffuse, ambient, specular, position, range, attenuation)
	{
		this->direction = direction;
		this->spot = spot;
		this->type = SPOT_LIGHT;
	}
};

// A struct containing all information pertaining to directional lights, to make the constant buffer better alligned
struct DirectionalLightBuffer
{
	XMFLOAT4	diffuse;            //16
	XMFLOAT4	ambient;            //32
	XMFLOAT4	specular;           //48
	XMFLOAT3	directionToLight;   //60
	LightType	type;                //64
	DirectionalLightBuffer()
	{
		diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
		ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
		specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
		directionToLight = XMFLOAT3(0.0f, 0.0f, 0.0f);
		type = DIRECTIONAL_LIGHT;
	}
	DirectionalLightBuffer(DirectionalLight directionalLight)
	{
		diffuse = directionalLight.diffuse;
		ambient = directionalLight.ambient;
		specular = directionalLight.specular;
		directionToLight = directionalLight.directionToLight;
		type = DIRECTIONAL_LIGHT;
	}
};

// A struct containing all information pertaining to point lights, to make the constant buffer better alligned
struct PointLightBuffer
{
	XMFLOAT4	diffuse;        //16
	XMFLOAT4	ambient;        //32
	XMFLOAT4	specular;       //48
	XMFLOAT3	position;       //60
	float		range;          //64
	XMFLOAT3	attenuation;    //76
	LightType   type;            //80
	PointLightBuffer()
	{
		diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
		ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
		specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
		position = XMFLOAT3(0.0f, 0.0f, 0.0f);
		range = 0.0f;
		attenuation = XMFLOAT3(0.0f, 0.0f, 0.0f);
		type = POINT_LIGHT;
	}
	PointLightBuffer(PointLight pointLight)
	{
		diffuse = pointLight.diffuse;
		ambient = pointLight.ambient;
		specular = pointLight.specular;
		position = pointLight.position;
		range = pointLight.range;
		attenuation = pointLight.attenuation;
		type = POINT_LIGHT;
	}
};

// A struct containing all information pertaining to spot lights, to make the constant buffer better alligned
struct SpotLightBuffer
{
	XMFLOAT4	diffuse;        //16
	XMFLOAT4	ambient;        //32
	XMFLOAT4	specular;       //48
	XMFLOAT3	position;       //60
	float		range;          //64
	XMFLOAT3	attenuation;    //76
	float		spot;           //80
	XMFLOAT3	direction;      //92
	LightType	type;            //96
	SpotLightBuffer()
	{
		diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
		ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
		specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
		position = XMFLOAT3(0.0f, 0.0f, 0.0f);
		range = 0.0f;
		attenuation = XMFLOAT3(0.0f, 0.0f, 0.0f);
		spot = 0.0f;
		direction = XMFLOAT3(0.0f, 0.0f, 0.0f);
		type = SPOT_LIGHT;
	}
	SpotLightBuffer(SpotLight spotLight)
	{
		diffuse = spotLight.diffuse;
		ambient = spotLight.ambient;
		specular = spotLight.specular;
		position = spotLight.position;
		range = spotLight.range;
		attenuation = spotLight.attenuation;
		spot = spotLight.spot;
		direction = spotLight.direction;
		type = SPOT_LIGHT;
	}
};