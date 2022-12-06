#include "Loading.h"

Mesh* LoadMesh(ID3D11Device* d3dDevice, char* path)
{
    Mesh* mesh = new Mesh;
    *mesh = OBJLoader::Load(path, d3dDevice);

    return mesh;
}

Texture* LoadTexture(ID3D11Device* d3dDevice, char* path)
{
    Texture* texture;

    //Convert the path to a wChar
    const size_t cSize = strlen(path) + 1;
    wchar_t* wPath = new wchar_t[cSize];
    mbstowcs(wPath, path, cSize);

    CreateDDSTextureFromFile(d3dDevice, wPath, nullptr, &texture);

    return texture;
}
