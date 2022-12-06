#include "Actor.h"

Actor::Actor(ID3D11Device* _pd3dDevice, char* path)
{
    m_pd3dDevice = _pd3dDevice;
    m_vertexBuffer = nullptr;
    m_indexBuffer = nullptr;
    HRESULT hr;
    hr = InitRenderedObject(path);
    if (FAILED(hr))
    {
        return;
    }
}

Actor::~Actor()
{
    if (m_vertexBuffer) m_vertexBuffer->Release();
    if (m_indexBuffer) m_indexBuffer->Release();
}

HRESULT Actor::InitRenderedObject()
{
    HRESULT hr;
    //The Rendered Object is a class that holds the vertices (position and normal) and indices of a shape.

    // Initialize the world matrix
    XMStoreFloat4x4(&m_world, XMMatrixIdentity());

    // Initialize vertex and index buffers
    m_indexBuffer = nullptr;
    m_vertexBuffer = nullptr;

    //Set up Vertices
    m_vertices =
    {
        
    };

    //Set up Indices
    m_indices =
    {
        
    };

    //Set up the normals by calculating them
    CalculateFlatNormals(&m_vertices, &m_indices);

    //Set up the lighting material
    m_material.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    m_material.ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
    m_material.specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    m_material.specularFalloff = 10.0f;

    //Init the cube's vertex buffer using the vertices and normals already set out in Vertices
    hr = InitVertexBuffer();
    if (FAILED(hr))
        return hr;
    //Init the cube's index buffer using the indices already set out in Indices
    hr = InitIndexBuffer();
    if (FAILED(hr))
        return hr;

    return S_OK;
}

HRESULT Actor::InitRenderedObject(char* path)
{
    HRESULT hr;
    //The Rendered Object is a class that holds the vertices (position and normal) and indices of a shape.

    // Initialize the world matrix
    XMStoreFloat4x4(&m_world, XMMatrixIdentity());

    // Initialize vertex and index buffers
    m_indexBuffer = nullptr;
    m_vertexBuffer = nullptr;

    m_mesh = OBJLoader::Load(path, m_pd3dDevice);

    //Set up the lighting material
    m_material.diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
    m_material.ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
    m_material.specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    m_material.specularFalloff = 10.0f;

    m_indexBuffer = m_mesh.IndexBuffer;
    m_indexCount = m_mesh.IndexCount;
    m_vertexBuffer = m_mesh.VertexBuffer;

    return S_OK;
}

HRESULT Actor::InitVertexBuffer()
{
    HRESULT hr;

    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SimpleVertex) * (m_vertices.size() + 1);
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA initData;
    ZeroMemory(&initData, sizeof(initData));
    //The .data() makes sure that the data is passed in as oppossed to the address of the vector container
    initData.pSysMem = m_vertices.data();

    hr = m_pd3dDevice->CreateBuffer(&bd, &initData, &m_vertexBuffer);

    if (FAILED(hr))
        return hr;

    return S_OK;
}

HRESULT Actor::InitIndexBuffer()
{
    HRESULT hr;

    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(WORD) * (m_indices.size());
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA initData;
    ZeroMemory(&initData, sizeof(initData));
    //The .data() makes sure that the data is passed in as oppossed to the address of the vector container
    initData.pSysMem = m_indices.data();
    hr = m_pd3dDevice->CreateBuffer(&bd, &initData, &m_indexBuffer);



    if (FAILED(hr))
        return hr;

    return S_OK;
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

    // Converts the XMFLOAT$X$ of the cube to an XMMATRIX
    XMMATRIX world = XMLoadFloat4x4(&m_world);
    // Transposes the matrix and copies it into the local constant buffer
    cb.mWorld = XMMatrixTranspose(world);

    //Materials:
    // copies the rendered diffuse material into the constant buffer
    cb.material.diffuse = m_material.diffuse;
    cb.material.ambient = m_material.ambient;
    cb.material.specular = m_material.specular;
    cb.material.specularFalloff = m_material.specularFalloff;

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