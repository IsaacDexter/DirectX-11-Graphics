#include "Camera.h"

Camera::~Camera()
{
}

Camera::Camera(XMFLOAT4 eye, XMFLOAT4 at, XMFLOAT4 up, float windowWidth, float windowHeight, float nearDepth, float farDepth)
{
    m_eye = eye;
    m_at = at;
    m_up = up;

    Reshape(windowWidth, windowHeight, nearDepth, farDepth);

    UpdateView();
    UpdateProjection();
}

XMFLOAT4 Camera::GetEye()
{
    return m_eye;
}

XMFLOAT4 Camera::GetAt()
{
    return m_at;
}

XMFLOAT4 Camera::GetUp()
{
    return m_up;
}

void Camera::SetPosition(XMFLOAT4 position)
{
    m_eye = position;
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


