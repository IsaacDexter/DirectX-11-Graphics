#include "Actor.h"

Actor::Actor(ID3D11Device* _pd3dDevice)
{
    m_pd3dDevice = _pd3dDevice;
    m_vertexBuffer = nullptr;
    m_indexBuffer = nullptr;
    HRESULT hr;
    hr = InitRenderedObject();
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

    //Set up Indice
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
    immediateContext->DrawIndexed(36, 0, 0);    //Draws the shape, total indices,starting index, starting vertex   
}



Pyramid::Pyramid(ID3D11Device* _p3dDevice) : Actor(_p3dDevice)
{
    InitRenderedObject();
}

HRESULT Pyramid::InitRenderedObject()
{
    HRESULT hr;
    //The Rendered Object is a class that holds the vertices (position and normal) and indices of a shape.

    // Initialize the world matrix
    XMStoreFloat4x4(&m_world, XMMatrixIdentity());

    // Initialize vertex and index buffers
    m_indexBuffer = nullptr;
    m_vertexBuffer = nullptr;

    //Set up Vertices of pyramid
    m_vertices =
    {
        { XMFLOAT3(-1.0f,   -1.0f,  1.0f),  XMFLOAT3(0.0f,  0.0f,   0.0f),  XMFLOAT2(0.0f,  1.0f) },    //Right Bottom Left
        { XMFLOAT3(1.0f,    -1.0f,  1.0f),  XMFLOAT3(0.0f,  0.0f,   0.0f),  XMFLOAT2(0.0f,  1.0f) },    //Back Bottom Left
        { XMFLOAT3(-1.0f,   -1.0f,  -1.0f), XMFLOAT3(0.0f,  0.0f,   0.0f),  XMFLOAT2(0.0f,  1.0f) },    //Front Bottom Left
        { XMFLOAT3(1.0f,    -1.0f,  -1.0f), XMFLOAT3(0.0f,  0.0f,   0.0f),  XMFLOAT2(1.0f,  1.0f) },    //Front Bottom Right
        { XMFLOAT3(0.0f,    1.0f,   0.0f),  XMFLOAT3(0.0f,  0.0f,   0.0f),  XMFLOAT2(0.5f,  0.0f) },    //Front Top Middle

        { XMFLOAT3(-1.0f,   -1.0f,  1.0f),  XMFLOAT3(0.0f,  0.0f,   0.0f),  XMFLOAT2(1.0f,  1.0f) },    //Back Bottom Right
        { XMFLOAT3(1.0f,    -1.0f,  1.0f),  XMFLOAT3(0.0f,  0.0f,   0.0f),  XMFLOAT2(1.0f,  1.0f) },    //Left Bottom Right
        { XMFLOAT3(-1.0f,   -1.0f,  -1.0f), XMFLOAT3(0.0f,  0.0f,   0.0f),  XMFLOAT2(1.0f,  1.0f) },    //Right Bottom Right
        { XMFLOAT3(1.0f,    -1.0f,  -1.0f), XMFLOAT3(0.0f,  0.0f,   0.0f),  XMFLOAT2(0.0f,  1.0f) },    //Left Bottom Left
        { XMFLOAT3(0.0f,    1.0f,   0.0f),  XMFLOAT3(0.0f,  0.0f,   0.0f),  XMFLOAT2(0.5f,  0.0f) },    //Right Top Middle

        { XMFLOAT3(-1.0f,   -1.0f,  1.0f),  XMFLOAT3(0.0f,  0.0f,   0.0f),  XMFLOAT2(0.0f,  1.0f) },    //Bottom Bottom Left
        { XMFLOAT3(1.0f,    -1.0f,  1.0f),  XMFLOAT3(0.0f,  0.0f,   0.0f),  XMFLOAT2(1.0f,  1.0f) },    //Bottom Bottom Right
        { XMFLOAT3(-1.0f,   -1.0f,  -1.0f), XMFLOAT3(0.0f,  0.0f,   0.0f),  XMFLOAT2(0.0f,  0.0f) },    //Bottom Top Left
        { XMFLOAT3(1.0f,    -1.0f,  -1.0f), XMFLOAT3(0.0f,  0.0f,   0.0f),  XMFLOAT2(1.0f,  0.0f) },    //Bottom Top Right
        { XMFLOAT3(0.0f,    1.0f,   0.0f),  XMFLOAT3(0.0f,  0.0f,   0.0f),  XMFLOAT2(0.5f,  0.0f) },    //Back Top Middle

        { XMFLOAT3(0.0f,    1.0f,   0.0f),  XMFLOAT3(0.0f,  0.0f,   0.0f),  XMFLOAT2(0.5f,  0.0f) },    //Left Top Middle
    };

    //Set up Indices of pyramid
    m_indices =
    {
        //Front:
        3,      2,      4,
        //Right:
        2 + 5,  0,      4 + 5,
        //Back:
        0 + 5,  1,      4 + 10,
        //Left:
        1 + 5,  3 + 5,  4 + 11,
        //Bottom:
        1 + 10, 2 + 10, 3 + 10,
        2 + 10, 1 + 10, 0 + 10,
    };
    //Set up the normals of the pyramid by calculating them
    CalculateFlatNormals(&m_vertices, &m_indices);


    //Set he diffuse material to reflect half red
    m_material.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    m_material.ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    m_material.specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
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

Cube::Cube(ID3D11Device* _p3dDevice) : Actor(_p3dDevice)
{
    InitRenderedObject();
}

HRESULT Cube::InitRenderedObject()
{
    HRESULT hr;
    //The Rendered Object is a class that holds the vertices (position and normal) and indices of a shape.

    // Initialize the world matrix
    XMStoreFloat4x4(&m_world, XMMatrixIdentity());

    // Initialize vertex and index buffers
    m_indexBuffer = nullptr;
    m_vertexBuffer = nullptr;

    //Set up Vertices of cube
    m_vertices =
    {
        { XMFLOAT3(-1.0f,  1.0f,   1.0f),  XMFLOAT3(0.0f,  0.0f,   0.0f),  XMFLOAT2(0.0f,  0.0f) },    //Right Top Left 
        { XMFLOAT3(1.0f,   1.0f,   1.0f),  XMFLOAT3(0.0f,  0.0f,   0.0f),  XMFLOAT2(1.0f,  0.0f) },    //Left Top Right
        { XMFLOAT3(-1.0f,  -1.0f,  1.0f),  XMFLOAT3(0.0f,  0.0f,   0.0f),  XMFLOAT2(0.0f,  1.0f) },    //Right Bottom Left
        { XMFLOAT3(1.0f,   -1.0f,  1.0f),  XMFLOAT3(0.0f,  0.0f,   0.0f),  XMFLOAT2(1.0f,  1.0f) },    //Left Bottom Right
        { XMFLOAT3(-1.0f,  1.0f,   -1.0f), XMFLOAT3(0.0f,  0.0f,   0.0f),  XMFLOAT2(0.0f,  0.0f) },    //Front Top Left
        { XMFLOAT3(1.0f,   1.0f,   -1.0f), XMFLOAT3(0.0f,  0.0f,   0.0f),  XMFLOAT2(1.0f,  0.0f) },    //Front Top Right
        { XMFLOAT3(-1.0f,  -1.0f,  -1.0f), XMFLOAT3(0.0f,  0.0f,   0.0f),  XMFLOAT2(0.0f,  1.0f) },    //Front Bottom Left
        { XMFLOAT3(1.0f,   -1.0f,  -1.0f), XMFLOAT3(0.0f,  0.0f,   0.0f),  XMFLOAT2(1.0f,  1.0f) },    //Front Bottom Right

        { XMFLOAT3(-1.0f,  1.0f,   1.0f),  XMFLOAT3(0.0f,   0.0f,   0.0f),  XMFLOAT2(1.0f,  0.0f) },    //Back Top Right
        { XMFLOAT3(1.0f,   1.0f,   1.0f),  XMFLOAT3(0.0f,   0.0f,   0.0f),  XMFLOAT2(0.0f,  0.0f) },    //Back Top Left
        { XMFLOAT3(-1.0f,  -1.0f,  1.0f),  XMFLOAT3(0.0f,   0.0f,   0.0f),  XMFLOAT2(1.0f,  1.0f) },    //Back Bottom Right
        { XMFLOAT3(1.0f,   -1.0f,  1.0f),  XMFLOAT3(0.0f,   0.0f,   0.0f),  XMFLOAT2(0.0f,  1.0f) },    //Back Bottom Left
        { XMFLOAT3(-1.0f,  1.0f,   -1.0f), XMFLOAT3(0.0f,   0.0f,   0.0f),  XMFLOAT2(1.0f,  0.0f) },    //Right Top Right
        { XMFLOAT3(1.0f,   1.0f,   -1.0f), XMFLOAT3(0.0f,   0.0f,   0.0f),  XMFLOAT2(0.0f,  0.0f) },    //Left Top Left
        { XMFLOAT3(-1.0f,  -1.0f,  -1.0f), XMFLOAT3(0.0f,   0.0f,   0.0f),  XMFLOAT2(1.0f,  1.0f) },    //Right Bottom Right
        { XMFLOAT3(1.0f,   -1.0f,  -1.0f), XMFLOAT3(0.0f,   0.0f,   0.0f),  XMFLOAT2(0.0f,  1.0f) },    //Left Bottom Left

        { XMFLOAT3(-1.0f,  1.0f,   1.0f),  XMFLOAT3(0.0f,   0.0f,   0.0f),  XMFLOAT2(0.0f,  0.0f) },    //Top Top Left
        { XMFLOAT3(1.0f,   1.0f,   1.0f),  XMFLOAT3(0.0f,   0.0f,   0.0f),  XMFLOAT2(1.0f,  0.0f) },    //Top Top Right
        { XMFLOAT3(-1.0f,  -1.0f,  1.0f),  XMFLOAT3(0.0f,   0.0f,   0.0f),  XMFLOAT2(0.0f,  1.0f) },    //Bottom Bottom Left
        { XMFLOAT3(1.0f,   -1.0f,  1.0f),  XMFLOAT3(0.0f,   0.0f,   0.0f),  XMFLOAT2(1.0f,  1.0f) },    //Bottom Bottom Right
        { XMFLOAT3(-1.0f,  1.0f,   -1.0f), XMFLOAT3(0.0f,   0.0f,   0.0f),  XMFLOAT2(0.0f,  1.0f) },    //Top Bottom Left
        { XMFLOAT3(1.0f,   1.0f,   -1.0f), XMFLOAT3(0.0f,   0.0f,   0.0f),  XMFLOAT2(1.0f,  1.0f) },    //Top Bottom Right
        { XMFLOAT3(-1.0f,  -1.0f,  -1.0f), XMFLOAT3(0.0f,   0.0f,   0.0f),  XMFLOAT2(0.0f,  0.0f) },    //Bottom Top Left
        { XMFLOAT3(1.0f,   -1.0f,  -1.0f), XMFLOAT3(0.0f,   0.0f,   0.0f),  XMFLOAT2(1.0f,  0.0f) },    //Bottom Top Right
    };

    //Set up Indices of cube
    m_indices =
    {
        //Front:
        5,      6,      4,
        6,      5,      7,
        //Right:
        4 + 8,  2,      0,
        2,      4 + 8,  6 + 8,
        //Left:
        1,      7 + 8,  5 + 8,
        7 + 8,  1,      3,
        //Back:
        0 + 8,  3 + 8,  1 + 8,
        3 + 8,  0 + 8,  2 + 8,
        //Top:
        1 + 16, 4 + 16, 0 + 16,
        4 + 16, 1 + 16, 5 + 16,
        //Bottom:
        7 + 16, 2 + 16, 6 + 16,
        2 + 16, 7 + 16, 3 + 16,
    };
    //Set up the normals of the cube by calculating them
    CalculateFlatNormals(&m_vertices, &m_indices);

    //Set up the cubes lighting material
    m_material.diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.5f);
    m_material.ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
    m_material.specular = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
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
