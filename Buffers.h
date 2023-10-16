#pragma once
#include <DirectXMath.h>

#include "Materials.h"
#include "Lights.h"

using namespace DirectX;

#define DIRECTIONAL_LIGHT_COUNT 8
#define POINT_LIGHT_COUNT 8
#define SPOT_LIGHT_COUNT 8

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
	SpotLightBuffer spotLights[SPOT_LIGHT_COUNT];

	// The position of the camera's eye in the world
	XMFLOAT4	EyeWorldPos;

	// counters for the amount of lights in the scene
	int directionalLightsCount;
	int pointLightsCount;
	int spotLightsCount;
	int pad;
};