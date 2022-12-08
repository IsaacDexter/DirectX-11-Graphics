#include "Level.h"

#pragma region Initialisation

Level::Level(char* path, ID3D11Device* d3dDevice, ID3D11DeviceContext* immediateContext, ID3D11Buffer* constantBuffer)
{
    m_d3dDevice = d3dDevice;
    m_immediateContext = immediateContext;
    m_constantBuffer = constantBuffer;
    Load(path);
}

void Level::InitObjects()
{
    _actors = new std::map<std::string, Actor*>();

    _actors->insert({ "cube", new Actor(_meshes->find("cube")->second, _materials->find("crate")->second, _textures->find("crateDiffuse")->second, _textures->find("crateSpecular")->second) });
    _actors->insert({ "cylinder", new Actor(_meshes->find("cylinder")->second, _materials->find("crate")->second, _textures->find("crateDiffuse")->second, _textures->find("crateSpecular")->second) });;
    _actors->find("cylinder")->second->SetPosition(XMFLOAT3(3.0f, 0.0f, 3.0f));

    //Initialise Lights
    LoadLights();

    //Initialise the camera
    m_camera = new Camera(XMFLOAT4(0.0f, 0.0f, -3.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f));
}

#pragma endregion

#pragma region Loading

void Level::Load(char* path)
{
    //Load meshes, textures and materials
    LoadMeshes();
    LoadMaterials();
    LoadTextures();

    //initialise objects
    InitObjects();

    // Initialize the view matrix
    XMVECTOR Eye = XMLoadFloat4(&m_camera->GetEye());
    XMVECTOR At = XMLoadFloat4(&m_camera->GetAt());
    XMVECTOR Up = XMLoadFloat4(&m_camera->GetUp());

    // Initialize the world matrix
    XMStoreFloat4x4(&m_world, XMMatrixIdentity());
    XMStoreFloat4x4(&m_view, XMMatrixLookAtLH(Eye, At, Up));
}

void Level::LoadTextures()
{
    _textures = new std::map<std::string, Texture*>();

    _textures->insert({ "crateDiffuse", LoadTexture(m_d3dDevice, "Textures/Crate_COLOR.dds") });
    _textures->insert({ "crateSpecular", LoadTexture(m_d3dDevice, "Textures/Crate_SPEC.dds") });
}

void Level::LoadMeshes()
{
    _meshes = new std::map<std::string, Mesh*>();

    _meshes->insert({ "cube", LoadMesh(m_d3dDevice, "Models/3dsMax/cube.obj") });
    _meshes->insert({ "cylinder", LoadMesh(m_d3dDevice, "Models/3dsMax/cylinder.obj") });
}

void Level::LoadMaterials()
{
    _materials = new std::map<std::string, Material*>();

    _materials->insert({ "crate", new Material(XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f), XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 10.0f) });
}

void Level::LoadLights()
{
    _directionalLights = new std::map<std::string, DirectionalLight*>();
    _pointLights = new std::map<std::string, PointLight*>();
    _spotLights = new std::map<std::string, SpotLight*>();

    _directionalLights->insert({ "sun", new DirectionalLight(XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f), XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f), XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f), XMFLOAT3(0.0f, 0.5f, -0.5f)) });

    _pointLights->insert({ "bulb", new PointLight(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT4(0.01f, 0.01f, 0.01f, 1.0f), XMFLOAT4(0.7f, 0.7f, 0.7f, 25.0f), XMFLOAT3(3.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.4f), 10.0f) });

    _spotLights->insert({ "torch", new SpotLight(XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f), XMFLOAT4(0.01f, 0.01f, 0.01f, 1.0f), XMFLOAT4(0.7f, 0.7f, 0.7, 20.0f), XMFLOAT3(0.0f, 0.0f, -3.0f), XMFLOAT3(0.0f, 0.0f, 0.4f), 10.0f, XMFLOAT3(-1.0f, -1.0f, 1.0f), 8.0f) });
}

#pragma endregion

#pragma region Updating

void Level::UpdateActors()
{
    // For each actor
    // Create a map iterator and point to beginning of map
    std::map<std::string, Actor*>::iterator it = _actors->begin();
    // Iterate over the map using Iterator till end.
    while (it != _actors->end())
    {
        // Access the actor from element pointed by it and call Update()
        it->second->Update();
        // Increment the Iterator to point to next entry
        it++;
    }
}

void Level::Update(float t)
{
    // Animate actors
    _actors->find("cube")->second->SetRotation(XMFLOAT3(t / 2, t, 0.0f));
    _actors->find("cylinder")->second->SetRotation(XMFLOAT3(-t, -t / 2, 0.0f));

    UpdateActors();
}

#pragma endregion

#pragma region Drawing

void Level::DrawActors(ConstantBuffer* cb)
{
    // For each actor
    // Create a map iterator and point to beginning of map
    std::map<std::string, Actor*>::iterator it = _actors->begin();
    // Iterate over the map using Iterator till end.
    while (it != _actors->end())
    {
        // Access the actor from element pointed by it and call Update()
        it->second->Draw(m_immediateContext, m_constantBuffer, *cb);
        // Increment the Iterator to point to next entry
        it++;
    }
}

void Level::StoreDirectionalLights(ConstantBuffer* cb)
{
    // For each directional Light
    // Initialise index to constant buffer
    int i = 0;
    // Create a map iterator and point to beginning of map
    std::map<std::string, DirectionalLight*>::iterator it = _directionalLights->begin();
    // Iterate over the map using Iterator till end.
    while (it != _directionalLights->end())
    {
        // Access the light from element pointed by it and store in the constant buffer
        cb->directionalLights[i] = *it->second;
        // Increment the Iterator to point to next entry
        it++;
        // Increment the index for the constant buffer
        i++;
    }
    // Store count into constant buffer
    cb->directionalLightsCount = i;
}

void Level::StorePointLights(ConstantBuffer* cb)
{
    // For each point Light
    // Initialise index to constant buffer
    int i = 0;
    // Create a map iterator and point to beginning of map
    std::map<std::string, PointLight*>::iterator it = _pointLights->begin();
    // Iterate over the map using Iterator till end.
    while (it != _pointLights->end())
    {
        // Access the light from element pointed by it and store in the constant buffer
        cb->pointLights[i] = *it->second;
        // Increment the Iterator to point to next entry
        it++;
        // Increment the index for the constant buffer
        i++;
    }
    cb->pointLightsCount = i;
}

void Level::StoreSpotLights(ConstantBuffer* cb)
{
    // For each spot Light
    // Initialise index to constant buffer
    int i = 0;
    // Create a map iterator and point to beginning of map
    std::map<std::string, SpotLight*>::iterator it = _spotLights->begin();
    // Iterate over the map using Iterator till end.
    while (it != _spotLights->end())
    {
        // Access the light from element pointed by it and store in the constant buffer
        cb->spotLights[i] = *it->second;
        // Increment the Iterator to point to next entry
        it++;
        // Increment the index for the constant buffer
        i++;
    }
    cb->spotLightsCount = i;
}

void Level::Draw(ConstantBuffer* cb)
{
    XMMATRIX world = XMLoadFloat4x4(&m_world);
    XMMATRIX view = XMLoadFloat4x4(&m_view);
    cb->mWorld = XMMatrixTranspose(world);
    cb->mView = XMMatrixTranspose(view);

    // Store lights in the constant buffer
    StoreDirectionalLights(cb);
    StorePointLights(cb);
    StoreSpotLights(cb);

    cb->EyeWorldPos = m_camera->GetEye();

    DrawActors(cb);
}

#pragma endregion

Level::~Level()
{
    _textures->clear();
    delete _textures;
    _meshes->clear();
    delete _meshes;
    _materials->clear();
    delete _materials;
    _actors->clear();
    delete _actors;
    _directionalLights->clear();
    delete _directionalLights;
    _pointLights->clear();
    delete _pointLights;
    _spotLights->clear();
    delete _spotLights;
    delete m_camera;
}