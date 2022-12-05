#pragma once
#include <DirectXMath.h>

#include "Materials.h"
#include "Lights.h"

using namespace DirectX;

#define DIRECTIONAL_LIGHT_COUNT 2
#define POINT_LIGHT_COUNT 1

struct ConstantBuffer
{
	// The world matrix
	XMMATRIX	mWorld;
	// The view matrix
	XMMATRIX	mView;
	// The projection matrix
	XMMATRIX	mProjection;

	MaterialBuffer material;

	// The first directional light in the scene
	DirectionalLightBuffer directionalLights[DIRECTIONAL_LIGHT_COUNT];
	PointLightBuffer pointLights[POINT_LIGHT_COUNT];

	// The position of the camera's eye in the world
	XMFLOAT4	EyeWorldPos;
};