#pragma once
#include <DirectXMath.h>

#include "Keyboard.h"
#include "Mouse.h"

using namespace DirectX;

class Camera
{
public:
	Camera(XMFLOAT4 eye, XMFLOAT4 at, XMFLOAT4 up, float windowWidth, float windowHeight, float nearDepth, float farDepth);
	~Camera();


	void Reshape(float windowWidth, float windowHeight, float nearDepth, float farDepth);

	XMFLOAT4X4 GetView();
	XMFLOAT4X4 GetProjection();
	XMFLOAT4X4 GetViewProjection();
	XMFLOAT4 GetEye();

	//Pure virtual
	virtual void Update(float t, Keyboard::KeyboardStateTracker keys, Mouse::ButtonStateTracker mouseButtons, XMFLOAT2 mousePosition, Mouse::Mode mouseMode);
protected:
	virtual void UpdateView();
	void UpdateProjection();

	void SetEye(XMFLOAT4 eye);
	void SetAt(XMFLOAT4 at);
	void SetUp(XMFLOAT4 up);
protected:

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

class FirstPersonCamera : Camera
{
public:
	FirstPersonCamera(XMFLOAT4 eye, XMFLOAT4 at, XMFLOAT4 up, float windowWidth, float windowHeight, float nearDepth, float farDepth);

	void Update(float t, Keyboard::KeyboardStateTracker keys, Mouse::ButtonStateTracker mouseButtons, XMFLOAT2 mousePosition, Mouse::Mode mouseMode) override;
private:
	void UpdateView() override;


	void Translate(XMFLOAT3 translation);
	void SetPosition(XMFLOAT3 newPosition);

	void Rotate(XMFLOAT3 rotation);
	void SetRotation(XMFLOAT3 newRotation);

	void LookTo(XMFLOAT3 direction);
	void LookAt(XMFLOAT4 at);

private:
	float m_movementSpeed;
	float m_rotationSpeed;

	XMFLOAT3 m_position;
	XMFLOAT3 m_rotation;

	XMFLOAT4 m_to;
};