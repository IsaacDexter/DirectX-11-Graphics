#pragma once
#include <DirectXMath.h>

using namespace DirectX;


struct SimpleVertex
{
	XMFLOAT3 Pos;
	XMFLOAT3 Normal;
	XMFLOAT2 TexCoord;
};

/// <summary>
/// Used for normal calculations.
/// </summary>
struct SimpleVertex_Vector
{
	XMVECTOR Pos;
	XMVECTOR Normal;
};