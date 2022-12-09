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
    //Initialise the camera
    m_camera = new Camera(XMFLOAT4(0.0f, 0.0f, -3.0f, 0.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f));
}

#pragma endregion

#pragma region Loading

void Level::LoadMesh(std::string name, std::string)
{
}

void Level::LoadMaterial(std::string name, std::string)
{
}

void Level::LoadMaterial(std::string name, XMFLOAT4 diffuse, XMFLOAT4 ambient, XMFLOAT4 specular, float specularFalloff)
{
}

void Level::LoadTexture(std::string name, std::string)
{
}

void Level::LoadActor(std::string name, std::string mesh, std::string material, std::string diffuseMap, std::string specularMap, XMFLOAT3 position, XMFLOAT3 rotation, XMFLOAT3 scale)
{
}

void Level::LoadLight(std::string name, XMFLOAT4 diffuse, XMFLOAT4 ambient, XMFLOAT4 specular, XMFLOAT3 direction)
{
}

void Level::LoadLight(std::string name, XMFLOAT4 diffuse, XMFLOAT4 ambient, XMFLOAT4 specular, XMFLOAT3 position, XMFLOAT3 attenuation, float range)
{
}

void Level::LoadLight(std::string name, XMFLOAT4 diffuse, XMFLOAT4 ambient, XMFLOAT4 specular, XMFLOAT3 position, XMFLOAT3 attenuation, float range, XMFLOAT3 direction, float spot)
{
}

void Level::LoadMeshes(json jFile)
{
    json meshes = jFile["meshes"];  //Gets the array
    int size = meshes.size();
    for (unsigned int i = 0; i < size; i++)
    {
        json meshDesc = meshes.at(i);
        std::string name = meshDesc["name"];    //Get the name
        std::string path = meshDesc["path"];    //Get the path of the mesh associated with that name
        LoadMesh(name, path);   //Append this mesh to the map
    }
}

void Level::LoadMaterials(json jFile)
{
    json materials = jFile["materials"];
    int size = materials.size();
    for (unsigned int i = 0; i < size; i++)
    {
        json materialsDesc = materials.at(i);
        std::string name = materialsDesc["name"];    //Get the name
        float diffuse_r = materialsDesc["diffuse_r"];
        float diffuse_g = materialsDesc["diffuse_g"];
        float diffuse_b = materialsDesc["diffuse_b"];
        float diffuse_a = materialsDesc["diffuse_a"];
        float ambient_r = materialsDesc["ambient_r"];
        float ambient_g = materialsDesc["ambient_g"];
        float ambient_b = materialsDesc["ambient_b"];
        float ambient_a = materialsDesc["ambient_a"];
        float specular_r = materialsDesc["specular_r"];
        float specular_g = materialsDesc["specular_g"];
        float specular_b = materialsDesc["specular_b"];
        float specular_a = materialsDesc["specular_a"];
        float specularFalloff = materialsDesc["specularFalloff"];
        LoadMaterial(name, XMFLOAT4(diffuse_r, diffuse_g, diffuse_b, diffuse_a), XMFLOAT4(ambient_r, ambient_g, ambient_b, ambient_a), XMFLOAT4(specular_r, specular_g, specular_b, specular_a), specularFalloff);   //Append this material to the map
    }
}

void Level::LoadTextures(json jFile)
{
    json textures = jFile["texures"];  //Gets the array
    int size = textures.size();
    for (unsigned int i = 0; i < size; i++)
    {
        json textureDesc = textures.at(i);
        std::string name = textureDesc["name"];    //Get the name
        std::string path = textureDesc["path"];    //Get the path of the texture associated with that name
        LoadTexture(name, path);   //Append this texture to the map
    }
}

void Level::LoadActors(json jFile)
{
    json actors = jFile["actors"];  //Gets the array
    int size = actors.size();
    for (unsigned int i = 0; i < size; i++)
    {
        json actorDesc = actors.at(i);
        std::string name = actorDesc["name"];    //Get the name
        std::string mesh = actorDesc["mesh"];    //Get the key to the mesh map for the actor's mesh
        std::string material = actorDesc["material"];    //Get the key to the material map for the actor's material
        std::string diffuseMap = actorDesc["diffuseMap"];    //Get the key to the texture map for the actor's diffusemap
        std::string specularMap = actorDesc["specularMap"];    //Get the key to the texture map for the actor's specularmap
        float position_x = actorDesc["position_x"];
        float position_y = actorDesc["position_y"];
        float position_z = actorDesc["position_z"];
        float rotation_x = actorDesc["rotation_x"];
        float rotation_y = actorDesc["rotation_y"];
        float rotation_z = actorDesc["rotation_z"];
        float scale_x = actorDesc["scale_x"];
        float scale_y = actorDesc["scale_y"];
        float scale_z = actorDesc["scale_z"];
        LoadActor(name, mesh, material, diffuseMap, specularMap, XMFLOAT3(position_x, position_y, position_z), XMFLOAT3(rotation_x, rotation_y, rotation_z), XMFLOAT3(scale_x, scale_y, scale_z));   //Append this actor to the map
    }
}

void Level::LoadLights(json jFile)
{
}

void Level::Load(char* path)
{
    //Initialise maps for storing of loaded data
    _meshes = new std::map<std::string, Mesh*>();
    _textures = new std::map<std::string, Texture*>();
    _materials = new std::map<std::string, Material*>();

    _directionalLights = new std::map<std::string, DirectionalLight*>();
    _pointLights = new std::map<std::string, PointLight*>();
    _spotLights = new std::map<std::string, SpotLight*>();

    _actors = new std::map<std::string, Actor*>();

    //Parse json
    json jFile;

    std::ifstream fileOpen("Levels/Level1.json");

    fileOpen >> jFile;

    std::string v = jFile["version"].get<std::string>();
    std::string levelName = jFile["name"];

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

//void Level::LoadTextures()
//{
//    _textures = new std::map<std::string, Texture*>();
//
//    _textures->insert({ "crateDiffuse", LoadTexture(m_d3dDevice, "Textures/Crate_COLOR.dds") });
//    _textures->insert({ "crateSpecular", LoadTexture(m_d3dDevice, "Textures/Crate_SPEC.dds") });
//}
//
//void Level::LoadMeshes()
//{
//    _meshes = new std::map<std::string, Mesh*>();
//
//    _meshes->insert({ "cube", LoadMesh(m_d3dDevice, "Models/3dsMax/cube.obj") });
//    _meshes->insert({ "cylinder", LoadMesh(m_d3dDevice, "Models/3dsMax/cylinder.obj") });
//}
//
//void Level::LoadMaterials()
//{
//    _materials = new std::map<std::string, Material*>();
//
//    _materials->insert({ "crate", new Material(XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f), XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 10.0f) });
//}
//
//void Level::LoadLights()
//{
//    _directionalLights = new std::map<std::string, DirectionalLight*>();
//    _pointLights = new std::map<std::string, PointLight*>();
//    _spotLights = new std::map<std::string, SpotLight*>();
//
//    _directionalLights->insert({ "sun", new DirectionalLight(XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f), XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f), XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f), XMFLOAT3(0.0f, 0.5f, -0.5f)) });
//
//    _pointLights->insert({ "bulb", new PointLight(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT4(0.01f, 0.01f, 0.01f, 1.0f), XMFLOAT4(0.7f, 0.7f, 0.7f, 25.0f), XMFLOAT3(3.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.4f), 10.0f) });
//
//    _spotLights->insert({ "torch", new SpotLight(XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f), XMFLOAT4(0.01f, 0.01f, 0.01f, 1.0f), XMFLOAT4(0.7f, 0.7f, 0.7, 20.0f), XMFLOAT3(0.0f, 0.0f, -3.0f), XMFLOAT3(0.0f, 0.0f, 0.4f), 10.0f, XMFLOAT3(-1.0f, -1.0f, 1.0f), 8.0f) });
//}
//
//void Level::LoadActors()
//{
//    _actors->insert({ "cube", new Actor(_meshes->find("cube")->second, _materials->find("crate")->second, _textures->find("crateDiffuse")->second, _textures->find("crateSpecular")->second) });
//    _actors->insert({ "cylinder", new Actor(_meshes->find("cylinder")->second, _materials->find("crate")->second, _textures->find("crateDiffuse")->second, _textures->find("crateSpecular")->second) });;
//    _actors->find("cylinder")->second->SetPosition(XMFLOAT3(3.0f, 0.0f, 3.0f));
//}

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