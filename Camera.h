#pragma once
#include <DirectXMath.h>

using namespace DirectX;

class Camera
{
public:
	Camera(XMFLOAT4 eye, XMFLOAT4 at, XMFLOAT4 up, float windowWidth, float windowHeight, float nearDepth, float farDepth);
	~Camera();

	/// <returns>The camera's Eye Position</returns>
	XMFLOAT4 GetEye();
	/// <returns>The camera's Focus Position</returns>
	XMFLOAT4 GetAt();
	/// <returns>The camera's Up Direction</returns>
	XMFLOAT4 GetUp();

	void SetPosition(XMFLOAT4 position);
	void SetAt(XMFLOAT4 at);
	void SetUp(XMFLOAT4 up);


	void Reshape(float windowWidth, float windowHeight, float nearDepth, float farDepth);

	XMFLOAT4X4 GetView();
	XMFLOAT4X4 GetProjection();
	XMFLOAT4X4 GetViewProjection();
private:
	void UpdateView();
	void UpdateProjection();
private:
	//View matrix
	XMFLOAT4 m_eye;
	XMFLOAT4 m_at;
	XMFLOAT4 m_up;

	XMFLOAT4X4 m_view;

	//Projection Matrix
	float m_windowWidth;
	float m_windowHeight;
	float m_nearDepth;
	float m_farDepth;

	XMFLOAT4X4 m_projection;
};