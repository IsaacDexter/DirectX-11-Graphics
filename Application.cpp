#include "Application.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message)
    {
        case WM_PAINT:
            hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

Application::Application()
{
	_hInst = nullptr;
	_hWnd = nullptr;
	_driverType = D3D_DRIVER_TYPE_NULL;
	_featureLevel = D3D_FEATURE_LEVEL_11_0;
	_pd3dDevice = nullptr;
	_pImmediateContext = nullptr;
	_pSwapChain = nullptr;
	_pRenderTargetView = nullptr;
	_pVertexShader = nullptr;
	_pPixelShader = nullptr;
	_pVertexLayout = nullptr;

    //_light = nullptr;

	_pConstantBuffer = nullptr;
}

Application::~Application()
{
	Cleanup();
}

HRESULT Application::Initialise(HINSTANCE hInstance, int nCmdShow)
{
    if (FAILED(InitWindow(hInstance, nCmdShow)))
	{
        return E_FAIL;
	}

    RECT rc;
    GetClientRect(_hWnd, &rc);
    _WindowWidth = rc.right - rc.left;
    _WindowHeight = rc.bottom - rc.top;

    if (FAILED(InitDevice()))
    {
        Cleanup();

        return E_FAIL;
    }

    // Initialize the view matrix
	XMVECTOR Eye = XMVectorSet(0.0f, 0.0f, -3.0f, 0.0f);
	XMVECTOR At = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    // Initialize the world matrix
    XMStoreFloat4x4(&_world, XMMatrixIdentity());

	XMStoreFloat4x4(&_view, XMMatrixLookAtLH(Eye, At, Up));

    // Initialize the projection matrix
    /*XMMatrixPerspective(  Top-down field of view angle in radians,
                            Aspect ratio of view-space X:Y,
                            Distance to the near clipping plane > 0,
                            Distance to the far clipping plane > 0),
                                                                        Returns: the perspective projection matrix  */
	XMStoreFloat4x4(&_projection, XMMatrixPerspectiveFovLH(XM_PIDIV2, _WindowWidth / (FLOAT) _WindowHeight, 0.01f, 100.0f));

	return S_OK;
}

HRESULT Application::InitShadersAndInputLayout()    //Loads in shaders from the HLSL (high-level shader language) file and returns an error if it fails
{
	HRESULT hr;

    // Compile the vertex shader
    ID3DBlob* pVSBlob = nullptr;
    hr = CompileShaderFromFile(L"DX11 Framework.hlsl", "VS", "vs_4_0", &pVSBlob);

    if (FAILED(hr))
    {
        MessageBox(nullptr,
                   L"The HLSL file cannot be compiled. Check VS Outpot for Error Log.", L"Error", MB_OK);
        return hr;
    }

	// Create the vertex shader
	hr = _pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &_pVertexShader);

	if (FAILED(hr))
	{	
		pVSBlob->Release();
        return hr;
	}

	// Compile the pixel shader
	ID3DBlob* pPSBlob = nullptr;
    hr = CompileShaderFromFile(L"DX11 Framework.hlsl", "PS", "ps_4_0", &pPSBlob);

    if (FAILED(hr))
    {
        MessageBox(nullptr,
            L"The HLSL file cannot be compiled. Check VS Outpot for Error Log.", L"Error", MB_OK);
        return hr;
    }

	// Create the pixel shader
	hr = _pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &_pPixelShader);
	pPSBlob->Release();

    if (FAILED(hr))
        return hr;

    // Define the input layout, which is used by the vertex 
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT numElements = ARRAYSIZE(layout);

    // Create the input layout
	hr = _pd3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
                                        pVSBlob->GetBufferSize(), &_pVertexLayout);
	pVSBlob->Release();

	if (FAILED(hr))
        return hr;

    // Set the input layout
    _pImmediateContext->IASetInputLayout(_pVertexLayout);

	return hr;
}

RenderedObject::RenderedObject(ID3D11Device* _pd3dDevice)
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

RenderedObject::~RenderedObject()
{
    if (m_vertexBuffer) m_vertexBuffer->Release();
    if (m_indexBuffer) m_indexBuffer->Release();
}

HRESULT RenderedObject::InitRenderedObject()
{
    HRESULT hr;
    //The Rendered Object is a class that holds the vertices (position and normal) and indices of a shape.

    // Initialize the world matrix
    XMStoreFloat4x4(&m_world, XMMatrixIdentity());

    // Initialize vertex and index buffers
    m_indexBuffer = nullptr;
    m_vertexBuffer = nullptr;

    //Set up Vertices of cube
    //m_vertices = 
    //{
    //    { XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f) },
    //    { XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f) },
    //    { XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f) },
    //    { XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f) },
    //    { XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f) },
    //    { XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f) },
    //    { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f) },
    //    { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f) },
    //};
    m_vertices = 
    {
        { XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f) },
        { XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f) },
        { XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f) },
        { XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f) },
        { XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f) },
        { XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f) },
        { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f) },
        { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f) },

        { XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f) },
        { XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f) },
        { XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f) },
        { XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f) },
        { XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f) },
        { XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f) },
        { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f) },
        { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f) },

        { XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f) },
        { XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f) },
        { XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f) },
        { XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f) },
        { XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f) },
        { XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f) },
        { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f) },
        { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f) },
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
    m_material.diffuse = XMFLOAT4(0.5f, 1.0f, 1.0f, 1.0f);

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

Pyramid::Pyramid(ID3D11Device* _pd3dDevice) : RenderedObject(_pd3dDevice)
{
}

Pyramid::~Pyramid()
{
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
    //m_vertices =
    //{
    //    { XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f) },
    //    { XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f) },
    //    { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f) },
    //    { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f) },
    //    { XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f) },
    //};
    m_vertices =
    {
        { XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f) },
        { XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f) },
        { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f) },
        { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f) },
        { XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f) },

        { XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f) },
        { XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f) },
        { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f) },
        { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f) },
        { XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f) },

        { XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f) },
        { XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f) },
        { XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f) },
        { XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f) },
        { XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f) },

        { XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f) },
    };

    //Set up Indices of pyramid
    m_indices =
    {
        //Front:
        3,      2,      4,
        //Left:
        2 + 5,  0,      4 + 5,
        //Back:
        0 + 5,  1,      4 + 10,
        //Right:
        1 + 5,  3 + 5,  4 + 11,
        //Base:
        1 + 10, 2 + 10, 3 + 10,
        2 + 10, 1 + 10, 0 + 10,
    };
    //Set up the normals of the pyramid by calculating them
    CalculateFlatNormals(&m_vertices, &m_indices);


    //Set he diffuse material to reflect half red
    m_material.diffuse = XMFLOAT4(1.0f, 1.0f, 0.5f, 1.0f);

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

HRESULT RenderedObject::InitVertexBuffer()
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

HRESULT RenderedObject::InitIndexBuffer()
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

/// <summary>
/// <para>Used to calculate the smooth shading normals by:</para>
/// <para>  -   computing the cross product for each triangle in the vertex</para>
/// <para>  -   add that to the normal of each vertex of that triangle</para>
/// <para>  -   normalize the normal of each vertex</para>
/// <para>This whole function is made possible thanks to Julien Guertault on stack exchange: https://computergraphics.stackexchange.com/a/4032 </para>
/// </summary>
/// <param name="Vertices">:    an std::vector* of SimpleVertex s. </param>
/// <param name="Indices">: an array of indices used to get the vertices of each triangle</param>
/// <param name="IndicesCount">:    the number of indices (i.e. the length of indices)</param>
void RenderedObject::CalculateSmoothNormals(std::vector<SimpleVertex>* Vertices, std::vector<WORD>* Indices)
{
    //  The function is based off of the following psuedocode
    //  for each traingle abc
    //      perpendicular - crossProduct(triangle.b - triangle.a, triangle.c - triangle.a)
    //      a.normal += perpendicular
    //      b.normal += perpendicular
    //      c.normal += perpendicular
    //  for each vertex
    //      vertex.normal = normalize(vertex.normal)

    //For each triangle...
    for (int i = 0; i < Indices->size(); i += 3)
    {
        //Load the positions into temporary vectors
        SimpleVertex_Vector a = { XMLoadFloat3(&Vertices->at(Indices->at(i      )).Pos), XMLoadFloat3(&Vertices->at(Indices->at(i     )).Normal)};
        SimpleVertex_Vector b = { XMLoadFloat3(&Vertices->at(Indices->at(i + 1  )).Pos), XMLoadFloat3(&Vertices->at(Indices->at(i + 1 )).Normal) };
        SimpleVertex_Vector c = { XMLoadFloat3(&Vertices->at(Indices->at(i + 2  )).Pos), XMLoadFloat3(&Vertices->at(Indices->at(i + 2 )).Normal) };

        //Find the perpendicular vector to the triangle
        XMVECTOR P = XMVector3Cross(b.Pos - a.Pos, c.Pos - a.Pos);

        //Add the result to the already exisiting normal and then store that result into the original vertex array's normal
        XMStoreFloat3(&Vertices->at(Indices->at(i       )).Normal, P + a.Normal);
        XMStoreFloat3(&Vertices->at(Indices->at(i + 1   )).Normal, P + b.Normal);
        XMStoreFloat3(&Vertices->at(Indices->at(i + 2   )).Normal, P + c.Normal);
    }
    //For each vertex's normal
    for (int i = 0; i < Vertices->size(); i++)
    {
        //Normalize that vertex's normal and store it where it was.
        XMStoreFloat3(&Vertices->at(i).Normal, XMVector3Normalize(XMLoadFloat3(&Vertices->at(i).Normal)));
    }
}

void RenderedObject::CalculateFlatNormals(std::vector<SimpleVertex>* Vertices, std::vector<WORD>* Indices)
{
    //  The function is based off of the following psuedocode
    //  for each traingle abc
    //      perpendicular = normalize(crossProduct(triangle.b - triangle.a, triangle.c - triangle.a))
    //      a.normal = perpendicular
    //      b.normal = perpendicular
    //      c.normal = perpendicular

    // For each triangle...
    int i = 0;
    while(i < Indices->size())
    {
        //Load the positions into temporary vectors
        SimpleVertex_Vector a = { XMLoadFloat3(&Vertices->at(Indices->at(i)).Pos), XMLoadFloat3(&Vertices->at(Indices->at(i)).Normal) };
        SimpleVertex_Vector b = { XMLoadFloat3(&Vertices->at(Indices->at(i + 1)).Pos), XMLoadFloat3(&Vertices->at(Indices->at(i + 1)).Normal) };
        SimpleVertex_Vector c = { XMLoadFloat3(&Vertices->at(Indices->at(i + 2)).Pos), XMLoadFloat3(&Vertices->at(Indices->at(i + 2)).Normal) };

        //Find the perpendicular vector to the triangle
        XMVECTOR P = XMVector3Normalize(XMVector3Cross(b.Pos - a.Pos, c.Pos - a.Pos));

        //Store that result to each vertex in the triangle and move onto the next triangle
        XMStoreFloat3(&Vertices->at(Indices->at(i)).Normal, P);
        XMStoreFloat3(&Vertices->at(Indices->at(i + 1)).Normal, P);
        XMStoreFloat3(&Vertices->at(Indices->at(i + 2)).Normal, P);
        i += 3;
    }
}

void RenderedObject::Update(XMMATRIX transform)
{
    XMStoreFloat4x4(&m_world, transform); //calculate a y rotation matrix and store _world
}

void RenderedObject::Draw(ID3D11DeviceContext* immediateContext, ID3D11Buffer* constantBuffer, ConstantBuffer cb )
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
    // copies the rendered diffuse material into the constant buffer
    cb.DiffMat = m_material.diffuse;

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

HRESULT Application::InitWindow(HINSTANCE hInstance, int nCmdShow)
{
    // Register class
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_TUTORIAL1);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW );
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"TutorialWindowClass";
    wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_TUTORIAL1);
    if (!RegisterClassEx(&wcex))
        return E_FAIL;

    // Create window
    _hInst = hInstance;
    RECT rc = {0, 0, 640, 480};
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    _hWnd = CreateWindow(L"TutorialWindowClass", L"DX11 Framework", WS_OVERLAPPEDWINDOW,
                         CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
                         nullptr);
    if (!_hWnd)
		return E_FAIL;

    ShowWindow(_hWnd, nCmdShow);

    return S_OK;
}

HRESULT Application::CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

    ID3DBlob* pErrorBlob;
    hr = D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel, 
        dwShaderFlags, 0, ppBlobOut, &pErrorBlob);

    if (FAILED(hr))
    {
        if (pErrorBlob != nullptr)
            OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());

        if (pErrorBlob) pErrorBlob->Release();

        return hr;
    }

    if (pErrorBlob) pErrorBlob->Release();

    return S_OK;
}

HRESULT Application::InitDevice()
{
    HRESULT hr = S_OK;

    UINT createDeviceFlags = 0;

#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };

    UINT numDriverTypes = ARRAYSIZE(driverTypes);

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };

	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

    //Define the render target output buffer
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 1;
    sd.BufferDesc.Width = _WindowWidth;
    sd.BufferDesc.Height = _WindowHeight;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = _hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

    // define  the depth/stencil buffer
    D3D11_TEXTURE2D_DESC depthStencilDesc;
    // Texture width
    depthStencilDesc.Width = _WindowWidth;
    // Texture height
    depthStencilDesc.Height = _WindowHeight;
    // Maximum number of mipmap levels in the texture
    depthStencilDesc.MipLevels = 1;
    // Number of textures in the array
    depthStencilDesc.ArraySize = 1;
    // Texture format
    depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    //      structure that specifies multisampling parameters for the texture
    depthStencilDesc.SampleDesc.Count = 1;
    depthStencilDesc.SampleDesc.Quality = 0;
    // identifies how the texture is to be read from and written to
    depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
    // flags for binding to pipeline stages
    depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    // flags to specify the types of cpu access allowed. 0 if cpu access is not required
    depthStencilDesc.CPUAccessFlags = 0;
    // misc flags.
    depthStencilDesc.MiscFlags = 0;

    for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
    {
        _driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDeviceAndSwapChain(nullptr, _driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
                                           D3D11_SDK_VERSION, &sd, &_pSwapChain, &_pd3dDevice, &_featureLevel, &_pImmediateContext);
        if (SUCCEEDED(hr))
            break;
    }

    if (FAILED(hr))
        return hr;

    // Create a render target view
    ID3D11Texture2D* pBackBuffer = nullptr;
    hr = _pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

    if (FAILED(hr))
        return hr;

    hr = _pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &_pRenderTargetView);
    pBackBuffer->Release();

    if (FAILED(hr))
        return hr;

    // create a rasterizer state to do wireframe rendering
    D3D11_RASTERIZER_DESC wfdesc;
    ZeroMemory(&wfdesc, sizeof(D3D11_RASTERIZER_DESC)); //Clears the size of memory need
    wfdesc.FillMode = D3D11_FILL_WIREFRAME; //Determines fill mode to use when rendering
    wfdesc.CullMode = D3D11_CULL_NONE;  //indicates that triangles facing the specified direction are not drawn (used in back face culling)
    hr = _pd3dDevice->CreateRasterizerState(&wfdesc, &_wireFrame);  //binds to the RS (render state) part of the pipeline, so created with CreateRasterizerState() method
    //First param (&wfdesc) is ther description of the render state, second is a pointer to a ID3D11RasterizerState object which holds the new render state

    // create a rasterizer state to do solid rendering
    D3D11_RASTERIZER_DESC sfdesc;
    ZeroMemory(&sfdesc, sizeof(D3D11_RASTERIZER_DESC)); //Clears the size of memory need
    sfdesc.FillMode = D3D11_FILL_SOLID; //Determines fill mode to use when rendering
    sfdesc.CullMode = D3D11_CULL_NONE;  //indicates that triangles facing the specified direction are not drawn (used in back face culling)
    hr = _pd3dDevice->CreateRasterizerState(&sfdesc, &_solidFill);  //binds to the RS (render state) part of the pipeline, so created with CreateRasterizerState() method
    //First param (&sfdesc) is ther description of the render state, second is a pointer to a ID3D11RasterizerState object which holds the new render state

    //Store the solid rasterizer state in _currentRasterizerState
    _currentRasterizerState = _solidFill;

    // Setup the viewport
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)_WindowWidth;
    vp.Height = (FLOAT)_WindowHeight;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    _pImmediateContext->RSSetViewports(1, &vp);

	hr = InitShadersAndInputLayout();
    if (FAILED(hr)) 
    { 
        return hr;
    }

    // Set primitive topology
    _pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    //initialise objects
    InitObjects();

    //Create the depth/stencil buffer
    _pd3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, &_depthStencilBuffer);
    //Create the depth/stencil view that will be bound to the OM stage of the pipeline
    _pd3dDevice->CreateDepthStencilView(_depthStencilBuffer, nullptr, &_depthStencilView);

    /* Binds one or more render targets and the depth stencil buffer to the outputMerger stage.OMSetRenderTargets(   Number of render targets to bind,
    *                                                                                                               Pointer to an array of render targets to bind to the device,
    *                                                                                                               Pointer to a depth-stencil view to bind to the device   )*/
    _pImmediateContext->OMSetRenderTargets(1, &_pRenderTargetView, _depthStencilView);

	// Create the constant buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
    hr = _pd3dDevice->CreateBuffer(&bd, nullptr, &_pConstantBuffer);

    

    if (FAILED(hr))
        return hr;

    return S_OK;
}

HRESULT Application::InitObjects()
{
    // Set vertex buffer

    //Initialise cube, including vertex and index buffers
    _cube = new RenderedObject(_pd3dDevice);

    //Initialise pyramid, including vertex and index buffers
    _pyramid = new Pyramid(_pd3dDevice);
    _pyramid->InitRenderedObject();

    _light = new Light();

    //initialse new light source
    _light->diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    //Make the light as above the camera facing into the scene
    _light->directionToLight = XMFLOAT3(0.0f, 0.5, -0.5f);

    return S_OK;
}

void Application::Cleanup()
{
    if (_pImmediateContext) _pImmediateContext->ClearState();
    if (_pConstantBuffer) _pConstantBuffer->Release();

    delete(_cube);
    _cube = nullptr;

    delete(_pyramid);
    _pyramid = nullptr;

    if (_depthStencilView) _depthStencilView->Release();
    if (_depthStencilBuffer) _depthStencilBuffer->Release();
    if (_pVertexLayout) _pVertexLayout->Release();
    if (_pVertexShader) _pVertexShader->Release();
    if (_pPixelShader) _pPixelShader->Release();
    if (_wireFrame) _wireFrame->Release();
    if (_pRenderTargetView) _pRenderTargetView->Release();
    if (_pSwapChain) _pSwapChain->Release();
    if (_pImmediateContext) _pImmediateContext->Release();
    if (_pd3dDevice) _pd3dDevice->Release();
}

void Application::Update()
{
    // Update our time
    static float t = 0.0f;

    if (_driverType == D3D_DRIVER_TYPE_REFERENCE)
    {
        t += (float) XM_PI * 0.0125f;
    }
    else
    {
        static DWORD dwTimeStart = 0;
        DWORD dwTimeCur = GetTickCount();

        if (dwTimeStart == 0)
            dwTimeStart = dwTimeCur;

        t = (dwTimeCur - dwTimeStart) / 1000.0f;
    }

    if (WM_KEYDOWN)
    {
        if ((GetAsyncKeyState(VK_LBUTTON) & 0x01))
        {
            if (_currentRasterizerState == _solidFill)
            {
                _currentRasterizerState = _wireFrame;
            }
            else
            {
                _currentRasterizerState = _solidFill;
            }
            _pImmediateContext->RSSetState(_currentRasterizerState); //Set the render state in out immediate context before any objects we want to render in that state (filled)

        }
    }

    //
    // Animate the cube
    //
	_cube->Update(XMMatrixRotationY(t) * XMMatrixRotationX(t * 0.5)); //calculate a y rotation matrix and store _world
    _pyramid->Update(XMMatrixRotationX(t) * XMMatrixTranslation(4, 0, 4)); //calculate a y rotation matrix and store in _world2. Translate it by 2, 0, 0 so its in a different world space.
}

void Application::Draw()
{
    //
    // Clear the back buffer
    //
    float ClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f };
    _pImmediateContext->ClearRenderTargetView(_pRenderTargetView, ClearColor);  // Clear the rendering target to blue
    _pImmediateContext->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    XMMATRIX world = XMLoadFloat4x4(&_world);
    XMMATRIX view = XMLoadFloat4x4(&_view);
    XMMATRIX projection = XMLoadFloat4x4(&_projection); //Load in infromation about our object
    //
    // Update variables
    //
    ConstantBuffer cb;
    cb.mWorld = XMMatrixTranspose(world);
    cb.mView = XMMatrixTranspose(view);
    cb.mProjection = XMMatrixTranspose(projection);
    cb.DiffLight = _light->diffuse;
    cb.DirToLight = _light->directionToLight;
    cb.DiffMat = XMFLOAT4(0.5f, 1.0f, 1.0f, 1.0f);

    _pImmediateContext->VSSetShader(_pVertexShader, nullptr, 0);
    _pImmediateContext->VSSetConstantBuffers(0, 1, &_pConstantBuffer);
    _pImmediateContext->PSSetConstantBuffers(0, 1, &_pConstantBuffer);
    _pImmediateContext->PSSetShader(_pPixelShader, nullptr, 0);

    _cube->Draw(_pImmediateContext, _pConstantBuffer, cb);
    _pyramid->Draw(_pImmediateContext, _pConstantBuffer, cb);

    _pSwapChain->Present(0, 0);
}
