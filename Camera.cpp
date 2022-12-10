#include "Camera.h"

Camera::~Camera()
{
}

Camera::Camera(XMFLOAT4 eye, XMFLOAT4 at, XMFLOAT4 up)
{
    m_eye = eye;
    m_at = at;
    m_up = up;

    XMStoreFloat4x4(&m_view, XMMatrixLookAtLH(XMLoadFloat4( &m_eye), XMLoadFloat4(&m_at), XMLoadFloat4(&m_up)));
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

XMFLOAT4X4 Camera::GetView()
{
    return m_view;
}
