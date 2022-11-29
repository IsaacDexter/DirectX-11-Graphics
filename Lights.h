#pragma once
#include <DirectXMath.h>

using namespace DirectX;


enum LightType
{
	DIRECTIONAL_LIGHT,
	POINT_LIGHT,
	SPOT_LIGHT,
};

/// <summary>Abstract light class upon which other lights are based</summary>
class Light
{
protected:
	LightType type;

	XMFLOAT4 diffuse;
	XMFLOAT4 ambient;
	XMFLOAT4 specular;
public:
	XMFLOAT4 getDiffuse();
	XMFLOAT4 getAmbient();
	XMFLOAT4 getSpecular();

	//Abstract, to be overridden
	virtual XMFLOAT3 getDirectionToLight() { return XMFLOAT3(); };
	virtual XMFLOAT3 getPosition() { return XMFLOAT3(); };
	virtual float getRange() { return 0.0f; };
	virtual XMFLOAT3 getAttenuation() { return XMFLOAT3(); };
};

class DirectionalLight : public Light
{
private:
	XMFLOAT3 directionToLight;
public:
	DirectionalLight(XMFLOAT4 diffuse, XMFLOAT4 ambient, XMFLOAT4 specular, XMFLOAT3 directionToLight);
	~DirectionalLight();
	XMFLOAT3 getDirectionToLight() override;
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
	XMFLOAT3 getPosition() override;
	/// <returns>range as a singular float</returns>
	float getRange() override;
	/// <returns>Attenuation packed into a 4D vector with padding</returns>
	XMFLOAT3 getAttenuation() override;
	PointLight(XMFLOAT4 diffuse, XMFLOAT4 ambient, XMFLOAT4 specular, XMFLOAT3 position, float range, XMFLOAT3 attenuation);
	~PointLight();
};