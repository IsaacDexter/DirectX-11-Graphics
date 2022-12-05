#pragma once
#include <DirectXMath.h>

#include "Materials.h"
#include "Lights.h"

using namespace DirectX;

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
	DirectionalLightBuffer directionalLights[2];

	// The position of the camera's eye in the world
	XMFLOAT4	EyeWorldPos;
};