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

    m_position = XMMatrixIdentity();
    m_rotation = XMMatrixIdentity();
    m_scale = XMMatrixIdentity();

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
    m_position *= XMMatrixTranslation(translation.x, translation.y, translation.z);
}

void Actor::Translate(XMMATRIX translation)
{
    m_position *= translation;
}

void Actor::SetPosition(XMFLOAT3 newPosition)
{
    m_position = XMMatrixTranslation(newPosition.x, newPosition.y, newPosition.z);
}

void Actor::SetPosition(XMMATRIX newPosition)
{
    m_position = newPosition;
}

void Actor::Rotate(XMFLOAT3 rotation)
{
    m_rotation *= XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
}

void Actor::Rotate(XMMATRIX rotation)
{
    m_rotation *= rotation;
}


void Actor::SetRotation(XMFLOAT3 newRotation)
{
    m_rotation = XMMatrixRotationRollPitchYaw(newRotation.x, newRotation.y, newRotation.z);
}

void Actor::SetRotation(XMMATRIX newRotation)
{
    m_rotation = newRotation;
}

void Actor::Scale(XMFLOAT3 scale)
{
    m_scale *= XMMatrixScaling(scale.x, scale.y, scale.z);
}

void Actor::Scale(XMMATRIX scale)
{
    m_scale *= scale;
}

void Actor::SetScale(XMFLOAT3 newScale)
{
    m_scale = XMMatrixScaling(newScale.x, newScale.y, newScale.z);
}

void Actor::SetScale(XMMATRIX newScale)
{
    m_scale = newScale;
}

void Actor::Transform(XMFLOAT3 translation, XMFLOAT3 rotation, XMFLOAT3 scale)
{
    Translate(translation);
    Rotate(rotation);
    Scale(scale);
}

void Actor::SetTransform(XMFLOAT3 newPosition, XMFLOAT3 newRotation, XMFLOAT3 newScale)
{
    SetPosition(newPosition);
    SetRotation(newRotation);
    SetScale(newScale);
}

#pragma endregion

void Actor::Update()
{
    XMStoreFloat4x4(&m_world, m_scale * m_rotation * m_position); //calculate translation matrix and store _world
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