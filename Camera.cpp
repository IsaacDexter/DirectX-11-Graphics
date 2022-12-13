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

void Camera::Update(float t, Keyboard::KeyboardStateTracker keys, Keyboard::State keyboard, Mouse::ButtonStateTracker mouseButtons, XMFLOAT2 mousePosition, Mouse::Mode mouseMode)
{

}


FirstPersonCamera::FirstPersonCamera(XMFLOAT4 eye, XMFLOAT4 to, XMFLOAT4 up, float windowWidth, float windowHeight, float nearDepth, float farDepth) : Camera(eye, to, up, windowWidth, windowHeight, nearDepth, farDepth)
{
    m_movementSpeed = 0.0075f;
    m_rotationSpeed = 0.0125f;

    m_position = XMFLOAT3(0.0f, 0.0f, 0.0f);
    m_rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
    m_to = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
    
    m_mousePos = XMFLOAT2(0.0f, 0.0f);

    

    SetDirection(XMFLOAT3(-to.x, -to.y, -to.z));
    SetPosition(XMFLOAT3(eye.x, eye.y, eye.z));
}

void FirstPersonCamera::Update(float t, Keyboard::KeyboardStateTracker keys, Keyboard::State keyboard, Mouse::ButtonStateTracker mouseButtons, XMFLOAT2 mousePosition, Mouse::Mode mouseMode)
{
    float mouseMovementX = mousePosition.x - m_mousePos.x;
    float mouseMovementY = mousePosition.y - m_mousePos.y;
    XMFLOAT3 rotationVector = XMFLOAT3(mouseMovementY * m_rotationSpeed, mouseMovementX * m_rotationSpeed, 0.0f);
    Rotate(rotationVector);

    XMFLOAT3 movementVector = XMFLOAT3(0.0f, 0.0f, 0.0f);

    movementVector.x = (keyboard.D - keyboard.A);
    movementVector.y = (keyboard.Space - keyboard.LeftControl);
    movementVector.z = (keyboard.S - keyboard.W);

    if (keyboard.P)
    {
        throw;
    }


    Translate(movementVector);

    m_mousePos = mousePosition;
    m_time = t;
}

void FirstPersonCamera::UpdateView()
{
    XMStoreFloat4x4(&m_view, XMMatrixLookToRH(XMLoadFloat4(&m_eye), XMLoadFloat4(&m_to), XMLoadFloat4(&m_up)));
}

void FirstPersonCamera::Translate(XMFLOAT3 translation)
{
    //Find new position for movement in z axis
    m_position.x += (translation.z * m_to.x * m_movementSpeed);
    m_position.y += (translation.z * m_to.y * m_movementSpeed);
    m_position.z += (translation.z * m_to.z * m_movementSpeed);
    //Find new position for movment in y axis 
    m_position.x += (translation.y * m_up.x * m_movementSpeed);
    m_position.y += (translation.y * m_up.y * m_movementSpeed);
    m_position.z += (translation.y * m_up.z * m_movementSpeed);
    //Find new position for movement in x axis
    XMFLOAT4 right;
    XMStoreFloat4(&right, XMVector3Cross(XMLoadFloat4(&m_to), XMLoadFloat4(&m_up)));
    m_position.x += (translation.x * right.x * m_movementSpeed);
    m_position.y += (translation.x * right.y * m_movementSpeed);
    m_position.z += (translation.x * right.z * m_movementSpeed);

    SetPosition(XMFLOAT3(m_position.x, m_position.y, m_position.z));
}

void FirstPersonCamera::SetPosition(XMFLOAT3 newPosition)
{
    m_position = newPosition;
    SetEye(XMFLOAT4(m_position.x, m_position.y, m_position.z, 0.0f));
}

void FirstPersonCamera::Rotate(XMFLOAT3 rotation)
{
    m_rotation.x += rotation.x;
    m_rotation.y += rotation.y;
    m_rotation.z += rotation.z;
    SetRotation(m_rotation);
}

void FirstPersonCamera::SetRotation(XMFLOAT3 newRotation)
{
    XMFLOAT3 direction = XMFLOAT3(sin(newRotation.y) * cos(newRotation.x), sin(newRotation.x), cos(-newRotation.y) * cos(newRotation.x));
    XMStoreFloat3(&direction, XMVector3Normalize(XMLoadFloat3(&direction)));
    SetDirection(direction);
}

void FirstPersonCamera::SetDirection(XMFLOAT3 direction)
{
    m_to.x = -direction.x;
    m_to.y = -direction.y;
    m_to.z = -direction.z;
    UpdateView();
}

OrbitingCamera::OrbitingCamera(XMFLOAT4 eye, XMFLOAT4 to, XMFLOAT4 up, float windowWidth, float windowHeight, float nearDepth, float farDepth) : Camera(eye, to, up, windowWidth, windowHeight, nearDepth, farDepth)
{

}

void OrbitingCamera::Update(float t, Keyboard::KeyboardStateTracker keys, Keyboard::State keyboard, Mouse::ButtonStateTracker mouseButtons, XMFLOAT2 mousePosition, Mouse::Mode mouseMode)
{
}
