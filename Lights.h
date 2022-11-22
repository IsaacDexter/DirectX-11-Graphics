#ifndef LIGHTS_H
#define LIGHTS_H

#include "Application.h"

/// <summary>Abstract light class upon which other lights are based</summary>
class Light
{
public:
	XMFLOAT4 getDiffuse();
	XMFLOAT4 getAmbient();
	XMFLOAT4 getSpecular();
protected:
	XMFLOAT4 diffuse;
	XMFLOAT4 ambient;
	XMFLOAT4 specular;
};

class DirectionalLight : public Light
{
public:
	DirectionalLight(XMFLOAT4 diffuse, XMFLOAT4 ambient, XMFLOAT4 specular, XMFLOAT4 directionToLight);
	~DirectionalLight();
	XMVECTOR getDirectionToLight();
protected:
	XMVECTOR directionToLight;
};

class PointLight : public Light
{
	XMFLOAT4	diffuse;
	XMFLOAT4	ambient;
	XMFLOAT4	specular;

	//Packed into 4D vector: (Position, Range)
	XMFLOAT3	position;
	FLOAT		range;

	//Packed into 4D vector: (A0, A1, A2, Pad)
	/// <summary></summary>
	XMFLOAT3	attenuation;
	FLOAT		pad;	//Pad the last float to set an array of lights if needed
};

#endif // !LIGHTS_H