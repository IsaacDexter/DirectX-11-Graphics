#include <DirectXMath.h>
#include <d3d11_1.h>

#include "OBJLoader.h"
#include "DDSTextureLoader.h"

using namespace DirectX;

typedef MeshData Mesh;
typedef ID3D11ShaderResourceView Texture;

Mesh* LoadOBJ(ID3D11Device* d3dDevice, std::string path);
Texture* LoadDDS(ID3D11Device* d3dDevice, std::string path);