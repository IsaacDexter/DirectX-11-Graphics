#include "Camera.h"

Camera::~Camera()
{
}

Camera::Camera(XMFLOAT4 eye, XMFLOAT4 at, XMFLOAT4 up, float windowWidth, float windowHeight, float nearDepth, float farDepth)
{
    m_eye = eye;
    m_at = at;
    m_up = up;
    UpdateView();

    Reshape(windowWidth, windowHeight, nearDepth, farDepth);
}

void Camera::SetEye(XMFLOAT4 eye)
{
    m_eye = eye;
    UpdateView();
}

void Camera::SetAt(XMFLOAT4 at)
{
    m_at = at;
    UpdateView();
}

void Camera::SetUp(XMFLOAT4 up)
{
    m_up = up;
    UpdateView();
}

void Camera::UpdateView()
{
    XMStoreFloat4x4(&m_view, XMMatrixLookAtLH(XMLoadFloat4(&m_eye), XMLoadFloat4(&m_at), XMLoadFloat4(&m_up)));
}

void Camera::UpdateProjection()
{
    // Initialize the projection matrix
    /*XMMatrixPerspective(  Top-down field of view angle in radians,
                            Aspect ratio of view-space X:Y,
                            Distance to the near clipping plane > 0,
                            Distance to the far clipping plane > 0),
                                                                        Returns: the perspective projection matrix  */
    XMStoreFloat4x4(&m_projection, XMMatrixPerspectiveFovLH(XM_PIDIV2, m_windowWidth / (float)m_windowHeight, m_nearDepth, m_farDepth));
}

void Camera::Reshape(float windowWidth, float windowHeight, float nearDepth, float farDepth)
{
    m_windowWidth = windowWidth;
    m_windowHeight = windowHeight;
    m_nearDepth = nearDepth;
    m_farDepth = farDepth;
    UpdateProjection();
}

XMFLOAT4X4 Camera::GetView()
{
    return m_view;
}

XMFLOAT4X4 Camera::GetProjection()
{
    return m_projection;
}

XMFLOAT4X4 Camera::GetViewProjection()
{
    XMFLOAT4X4 returnMatrix;
    XMStoreFloat4x4(&returnMatrix, XMMatrixMultiply(XMLoadFloat4x4(&m_view), XMLoadFloat4x4(&m_projection)));
    return returnMatrix;
}

XMFLOAT4 Camera::GetEye()
{
    return m_eye;
}

void Camera::Update(float t, Keyboard::KeyboardStateTracker keys, Mouse::ButtonStateTracker mouseButtons, XMFLOAT2 mousePosition, Mouse::Mode mouseMode)
{

}


FirstPersonCamera::FirstPersonCamera(XMFLOAT4 eye, XMFLOAT4 at, XMFLOAT4 up, float windowWidth, float windowHeight, float nearDepth, float farDepth) : Camera(eye, at, up, windowWidth, windowHeight, nearDepth, farDepth)
{
    m_movementSpeed = 0.07f;
    m_rotationSpeed = 0.004f;

    m_position = XMFLOAT3(0.0f, 0.0f, 0.0f);
    m_rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
    
    SetPosition(XMFLOAT3(eye.x, eye.y, eye.z));
    LookAt(at);

    XMFLOAT4 m_to;
}

void FirstPersonCamera::Update(float t, Keyboard::KeyboardStateTracker keys, Mouse::ButtonStateTracker mouseButtons, XMFLOAT2 mousePosition, Mouse::Mode mouseMode)
{
    if (keys.pressed.Left)
    {
        Rotate(XMFLOAT3(-0.004f, 0.0f, 0.0f));
    }
    if (keys.pressed.Left)
    {
        Rotate(XMFLOAT3(-0.004f, 0.0f, 0.0f));
    }
}

void FirstPersonCamera::UpdateView()
{
    XMStoreFloat4x4(&m_view, XMMatrixLookToRH(XMLoadFloat4(&m_eye), XMLoadFloat4(&m_to), XMLoadFloat4(&m_up)));
}

void FirstPersonCamera::Translate(XMFLOAT3 translation)
{
    SetPosition(XMFLOAT3(m_position.x + translation.x, m_position.y + translation.y, m_position.z + translation.z));
}

void FirstPersonCamera::SetPosition(XMFLOAT3 newPosition)
{
    m_position = newPosition;
    SetEye(XMFLOAT4(m_position.x, m_position.y, m_position.z, 0.0f));
}

void FirstPersonCamera::Rotate(XMFLOAT3 rotation)
{
    SetRotation(XMFLOAT3(m_rotation.x + rotation.x, m_rotation.z + rotation.z, m_rotation.z + rotation.z));
}

void FirstPersonCamera::SetRotation(XMFLOAT3 newRotation)
{
    m_rotation = newRotation;
    LookTo(m_rotation);
}

void FirstPersonCamera::LookTo(XMFLOAT3 direction)
{
    m_to.x = direction.x;
    m_to.y = direction.y;
    m_to.z = direction.z;
    UpdateView();
}

void FirstPersonCamera::LookAt(XMFLOAT4 at)
{
    // Get the direction the camera is initially looking in from the constructor
    XMVECTOR direction = XMVectorSet(at.x - m_position.x, at.y - m_position.y, at.z - m_position.z, 1.0f);
    direction = XMVector4Normalize(direction);

    // Store that normalised direction in m_rotation
    XMFLOAT4 rotation;
    XMStoreFloat4(&rotation, direction);
    SetRotation(XMFLOAT3(rotation.x, rotation.y, rotation.z));
}
