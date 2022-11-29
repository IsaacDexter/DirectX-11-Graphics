#pragma once
#include <DirectXMath.h>

using namespace DirectX;

/// <summary>Abstract light class upon which other lights are based</summary>
class Light
{
protected:
	XMFLOAT4 diffuse;
	XMFLOAT4 ambient;
	XMFLOAT4 specular;
public:
	XMFLOAT4 getDiffuse();
	XMFLOAT4 getAmbient();
	XMFLOAT4 getSpecular();
};

class DirectionalLight : public Light
{
private:
	XMFLOAT3 directionToLight;
public:
	DirectionalLight(XMFLOAT4 diffuse, XMFLOAT4 ambient, XMFLOAT4 specular, XMFLOAT3 directionToLight);
	~DirectionalLight();
	XMFLOAT3 getDirectionToLight();
};

class PointLight : public Light
{
private:
	//Packed into 4D vector: (Position, Range)
	XMFLOAT3	position;
	float		range;
	//Packed into 4D vector: (A0, A1, A2, Pad)
	/// <summary></summary>
	XMFLOAT3	attenuation;
public:
	/// <returns>Position and range to be packed into a 4D vector</returns>
	XMFLOAT3 getPosition();
	/// <returns>range as a singular float</returns>
	float getRange();
	/// <returns>Attenuation packed into a 4D vector with padding</returns>
	XMFLOAT3 getAttenuation();
	PointLight(XMFLOAT4 diffuse, XMFLOAT4 ambient, XMFLOAT4 specular, XMFLOAT3 position, float range, XMFLOAT3 attenuation);
	~PointLight();
};