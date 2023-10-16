#include "Actor.h"

Actor::Actor(Mesh* mesh, Material* material, Texture* diffuseMap, Texture* specularMap, XMFLOAT3 position, XMFLOAT3 rotation, XMFLOAT3 scale)
{ 

    m_mesh = mesh;
    m_material = material;
    m_diffuseMap = diffuseMap;
    m_specularMap = specularMap;

    //Load vertex and index buffers from the mesh passed in
    m_indexBuffer = m_mesh->IndexBuffer;
    m_indexCount = m_mesh->IndexCount;
    m_vertexBuffer = m_mesh->VertexBuffer;

    //Set default translation matrices
    XMStoreFloat4x4(&m_world, XMMatrixIdentity());

    m_position = XMFLOAT3(0.0f, 0.0f, 0.0f);
    m_rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
    m_scale = XMFLOAT3(0.0f, 0.0f, 0.0f);

    SetTransform(position, rotation, scale);
}

Actor::~Actor()
{
    if (m_vertexBuffer) m_vertexBuffer->Release();
    if (m_indexBuffer) m_indexBuffer->Release();
}

#pragma region Translation

void Actor::Translate(XMFLOAT3 translation)
{
    m_position = Add(m_position, translation);
    UpdateTransform();
}

void Actor::SetPosition(XMFLOAT3 newPosition)
{
    m_position = newPosition;
    UpdateTransform();
}

XMFLOAT3 Actor::GetPosition()
{
    return m_position;
}

void Actor::Rotate(XMFLOAT3 rotation)
{
    m_rotation = Add(m_rotation, rotation);
    UpdateTransform();
}

void Actor::SetRotation(XMFLOAT3 newRotation)
{
    m_rotation = newRotation;
    UpdateTransform();
}

XMFLOAT3 Actor::GetRotation()
{
    return m_rotation;
}

void Actor::Scale(XMFLOAT3 scale)
{
    m_scale = Add(m_scale, scale);
    UpdateTransform();
}

void Actor::SetScale(XMFLOAT3 newScale)
{
    m_scale = newScale;
    UpdateTransform();
}

XMFLOAT3 Actor::GetScale()
{
    return m_scale;
}

void Actor::Transform(XMFLOAT3 translation, XMFLOAT3 rotation, XMFLOAT3 scale)
{
    Translate(translation);
    Rotate(rotation);
    Scale(scale);
    UpdateTransform();
}

void Actor::SetTransform(XMFLOAT3 newPosition, XMFLOAT3 newRotation, XMFLOAT3 newScale)
{
    SetPosition(newPosition);
    SetRotation(newRotation);
    SetScale(newScale);
    UpdateTransform();
}

void Actor::SetTexture(Texture* texture)
{
    m_diffuseMap = texture;
}

#pragma endregion

void Actor::UpdateTransform()
{
    XMMATRIX scale = XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);
    XMMATRIX rotation = XMMatrixRotationRollPitchYaw(m_rotation.x, m_rotation.y, m_rotation.z);
    XMMATRIX translation = XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
    XMStoreFloat4x4(&m_world, scale * rotation * translation); //calculate translation matrix and store _world
}

void Actor::Update()
{

}

void Actor::Draw(ID3D11DeviceContext* immediateContext, ID3D11Buffer* constantBuffer, ConstantBuffer cb)
{
    UINT stride = sizeof(SimpleVertex);
    UINT offset = 0;
    //Load the pyramid's vertex and index buffers into the immediate context
    immediateContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
    immediateContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R16_UINT, 0);

    //Binds textures
    immediateContext->PSSetShaderResources(0, 1, &m_diffuseMap);
    immediateContext->PSSetShaderResources(1, 1, &m_specularMap);

    // Converts the XMFLOAT$X$ of the cube to an XMMATRIX
    XMMATRIX world = XMLoadFloat4x4(&m_world);
    // Transposes the matrix and copies it into the local constant buffer
    cb.mWorld = XMMatrixTranspose(world);

    //Materials:
    // copies the rendered diffuse material into the constant buffer
    cb.material.diffuse = m_material->diffuse;
    cb.material.ambient = m_material->ambient;
    cb.material.specular = m_material->specular;
    cb.material.specularFalloff = m_material->specularFalloff;

    // Set vertex buffer
    immediateContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

    // Set index buffer
    immediateContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R16_UINT, 0);


    /*Copies the local constant buffer into the constant buffer on the GPU. UpdateSubresource(  a pointer to the destination resource,
                                                                                                    a zero based index that identifies the destination subresource,
                                                                                                    A box that defines the portion of the destination subresource to copy the resource data into. If NULL, the data is written to the destination subresource with no offset,
                                                                                                    A pointer to the source data memory,
                                                                                                    the size of one depth slice of source data  )*/
    immediateContext->UpdateSubresource(constantBuffer, 0, nullptr, &cb, 0, 0);

    //
    // Renders a triangle
    //

    //Draws the object with the new world matrix
    immediateContext->DrawIndexed(m_indexCount, 0, 0);    //Draws the shape, total indices,starting index, starting vertex   
}

XMFLOAT3 Actor::Add(XMFLOAT3 a, XMFLOAT3 b)
{
    return XMFLOAT3(a.x + b.y, a.y + b.y, a.z + b.z);
}
