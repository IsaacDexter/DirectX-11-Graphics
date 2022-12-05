#pragma once
#include <DirectXMath.h>

using namespace DirectX;

struct Material
{
	XMFLOAT4 diffuse;
	XMFLOAT4 ambient;
	XMFLOAT4 specular;
	//The Power to raise specular falloff by. Defaults to 10.
	float	 specularFalloff;
};

// A struct containing all information pertaining to lighting materials, to make the constant buffer better alligned
struct MaterialBuffer
{
	XMFLOAT4	diffuse;            //16
	XMFLOAT4	ambient;            //32
	XMFLOAT4	specular;           //48
	float		specularFalloff;	//52
	XMFLOAT3	pad;				//64
	MaterialBuffer()
	{
		diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
		ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
		specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
		specularFalloff = 0.0f;
	}
	MaterialBuffer(Material material)
	{
		diffuse = material.diffuse;
		ambient = material.ambient;
		specular = material.specular;
		specularFalloff = material.specularFalloff;
	}
};