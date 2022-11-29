#include "Lights.h"

XMFLOAT4 Light::getAmbient()
{
	return ambient;
}

XMFLOAT4 Light::getDiffuse()
{
	return diffuse;
}

XMFLOAT4 Light::getSpecular()
{
	return specular;
}

DirectionalLight::DirectionalLight(XMFLOAT4 diffuse, XMFLOAT4 ambient, XMFLOAT4 specular, XMFLOAT3 directionToLight)
{
	this->diffuse = diffuse;
	this->ambient = ambient;
	this->specular = specular;
	this->directionToLight = directionToLight;
}

DirectionalLight::~DirectionalLight()
{
}

XMFLOAT3 DirectionalLight::getDirectionToLight()
{
	return directionToLight;
}

XMFLOAT3 PointLight::getPosition()
{
	return position;
}

float PointLight::getRange()
{
	return range;
}

XMFLOAT3 PointLight::getAttenuation()
{
	return attenuation;
}

PointLight::PointLight(XMFLOAT4 diffuse, XMFLOAT4 ambient, XMFLOAT4 specular, XMFLOAT3 position, float range, XMFLOAT3 attenuation)
{
	this->diffuse = diffuse;
	this->ambient = ambient;
	this->specular = specular;
	this->position = position;
	this->range = range;
	this->attenuation = attenuation;
}

PointLight::~PointLight()
{
}
