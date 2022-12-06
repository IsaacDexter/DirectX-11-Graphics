#include "Actor.h"

Actor::Actor(ID3D11Device* _pd3dDevice, Mesh* mesh, Material* material, Texture* diffuseMap, Texture* specularMap = nullptr)
{ 
    m_pd3dDevice = _pd3dDevice;

    m_mesh = mesh;
    m_material = material;
    m_diffuseMap = diffuseMap;
    m_specularMap = specularMap;

    m_indexBuffer = m_mesh->IndexBuffer;
    m_indexCount = m_mesh->IndexCount;
    m_vertexBuffer = m_mesh->VertexBuffer;
}

Actor::~Actor()
{
    if (m_vertexBuffer) m_vertexBuffer->Release();
    if (m_indexBuffer) m_indexBuffer->Release();
}

void Actor::Update(XMMATRIX transform)
{
    XMStoreFloat4x4(&m_world, transform); //calculate a y rotation matrix and store _world
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