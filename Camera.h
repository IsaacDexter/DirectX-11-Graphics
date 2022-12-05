#pragma once
#include <DirectXMath.h>

using namespace DirectX;

class Camera
{
public:
	Camera(XMFLOAT4 eye, XMFLOAT4 at, XMFLOAT4 up);
	~Camera();

	/// <returns>The camera's Eye Position</returns>
	XMFLOAT4 GetEye();
	/// <returns>The camera's Focus Position</returns>
	XMFLOAT4 GetAt();
	/// <returns>The camera's Up Direction</returns>
	XMFLOAT4 GetUp();

private:
	XMFLOAT4 m_eye;
	XMFLOAT4 m_at;
	XMFLOAT4 m_up;
};