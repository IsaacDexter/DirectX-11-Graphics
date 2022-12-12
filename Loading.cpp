#include "Loading.h"

Mesh* LoadOBJ(ID3D11Device* d3dDevice, std::string path)
{
    Mesh* mesh = new Mesh;
    *mesh = OBJLoader::Load(path, d3dDevice, true);

    return mesh;
}

Texture* LoadDDS(ID3D11Device* d3dDevice, std::string path)
{
    Texture* texture;

    //Convert the path to a wChar
    const char* c = path.c_str();
    const size_t cSize = strlen(c) + 1;
    wchar_t* wPath = new wchar_t[cSize];
    mbstowcs(wPath, c, cSize);

    CreateDDSTextureFromFile(d3dDevice, wPath, nullptr, &texture);

    return texture;
}
