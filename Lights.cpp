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

DirectionalLight::DirectionalLight(XMFLOAT4 diffuse, XMFLOAT4 ambient, XMFLOAT4 specular, XMFLOAT4 directionToLight)
{
	this->diffuse = diffuse;
	this->ambient = ambient;
	this->specular = specular;
	this->directionToLight = XMLoadFloat4(&directionToLight);
}

DirectionalLight::~DirectionalLight()
{
}

XMVECTOR DirectionalLight::getDirectionToLight()
{
	return directionToLight;
}
