#include "Billboard.h"

Billboard::Billboard(XMFLOAT3 position, XMFLOAT2 size, Texture* texture, Material* material, ID3D11Device* d3dDevice)
{
    

    //Set default translation matrices
    XMStoreFloat4x4(&m_world, XMMatrixIdentity());

    SetPosition(position);
    m_size = size;
    m_diffuseMap = texture;
    m_material = material;
    m_d3dDevice = d3dDevice;

    m_vertexBufferDesc;
    ZeroMemory(&m_vertexBufferDesc, sizeof(m_vertexBufferDesc));
	m_vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_vertexBufferDesc.ByteWidth = sizeof(SimpleVertex) * 8;
	m_vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_vertexBufferDesc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA InitData;
    ZeroMemory(&InitData, sizeof(InitData));

	InitData.pSysMem = m_vertices;
    m_d3dDevice->CreateBuffer(&m_vertexBufferDesc, &InitData, &m_vertexBuffer);

    // Create index buffer
	WORD indices[6];

    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(WORD) * 36;
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;

    ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = indices;
    m_d3dDevice->CreateBuffer(&bd, &InitData, &m_indexBuffer);
}

void Billboard::Update(XMFLOAT3 cameraPos)
{
    //Calculate billboards new vectors
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);
	m_normal = XMFLOAT3(cameraPos.x - m_position.x, cameraPos.y - m_position.y, cameraPos.z - m_position.z);
	XMVECTOR to = XMLoadFloat3(&m_normal);
	XMVECTOR right = XMVector3Normalize(XMVector3Cross(up, to));
	XMStoreFloat3(&m_right, right);

    //Calculate billboards new vertices
	SimpleVertex sw;
	sw.Pos = XMFLOAT3(m_position.x + m_right.x * (m_size.x / 2), m_position.y, m_position.z + m_right.z * (m_size.x / 2));
	sw.Normal = m_normal;
	sw.TexCoord = XMFLOAT2(0.0f, 1.0f);

	SimpleVertex se;
	se.Pos = XMFLOAT3(m_position.x - m_right.x * (m_size.x / 2), m_position.y, m_position.z - m_right.z * (m_size.x / 2));
	se.Normal = m_normal;
	se.TexCoord = XMFLOAT2(1.0f, 1.0f);

	SimpleVertex nw;
	nw.Pos = XMFLOAT3(m_position.x + m_right.x * (m_size.x / 2), m_position.y + m_size.y, m_position.z + m_right.z * (m_size.x / 2));
	nw.Normal = m_normal;
	nw.TexCoord = XMFLOAT2(0.0f, 0.0f);

	SimpleVertex ne;
	ne.Pos = XMFLOAT3(m_position.x - m_right.x * (m_size.x / 2), m_position.y + m_size.y, m_position.z - m_right.z * (m_size.x / 2));
	ne.Normal = m_normal;
	ne.TexCoord = XMFLOAT2(1.0f, 0.0f);

	m_vertices[0] = nw;
	m_vertices[1] = ne;
	m_vertices[2] = sw;
	m_vertices[3] = se;

	//Update vertex buffer to reflect this data
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = m_vertices;
    m_d3dDevice->CreateBuffer(&m_vertexBufferDesc, &InitData, &m_vertexBuffer);
}

void Billboard::Draw(ID3D11DeviceContext* immediateContext, ID3D11Buffer* constantBuffer, ConstantBuffer cb)
{
    UINT stride = sizeof(SimpleVertex);
    UINT offset = 0;
    //Load the pyramid's vertex and index buffers into the immediate context
    immediateContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
    immediateContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R16_UINT, 0);

    //Binds textures
    immediateContext->PSSetShaderResources(0, 1, &m_diffuseMap);
    immediateContext->PSSetShaderResources(1, 1, &m_diffuseMap);

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
    immediateContext->DrawIndexed(4, 0, 0);    //Draws the shape, total indices,starting index, starting vertex   
}

void Billboard::SetPosition(XMFLOAT3 newPosition)
{
    m_position = newPosition;
    //Update world matrix with new position
    XMStoreFloat4x4(&m_world, XMLoadFloat4x4(&m_world) * XMMatrixTranslation(m_position.x, m_position.y, m_position.z));
}
