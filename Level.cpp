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

void Level::LoadMesh(std::string name, std::string path)
{
    _meshes->insert({ name, LoadOBJ(m_d3dDevice, path) });
}

void Level::LoadMaterial(std::string name, std::string path)
{
}

void Level::LoadMaterial(std::string name, XMFLOAT4 diffuse, XMFLOAT4 ambient, XMFLOAT4 specular, float specularFalloff)
{
    _materials->insert({ name, new Material(    diffuse,
                                                ambient,
                                                specular,
                                                specularFalloff) });
}

void Level::LoadTexture(std::string name, std::string path)
{

    _textures->insert({ name, LoadDDS(m_d3dDevice, path) });
}

void Level::LoadActor(std::string name, std::string mesh, std::string material, std::string diffuseMap, std::string specularMap, XMFLOAT3 position, XMFLOAT3 rotation, XMFLOAT3 scale)
{
    _actors->insert({ name, new Actor(  _meshes->find(mesh)->second,
                                        _materials->find(material)->second,
                                        _textures->find(diffuseMap)->second,
                                        _textures->find(specularMap)->second,
                                        position,
                                        rotation,
                                        scale   ) });
}

void Level::LoadDirectionalLight(std::string name, XMFLOAT4 diffuse, XMFLOAT4 ambient, XMFLOAT4 specular, XMFLOAT3 direction)
{
    _directionalLights->insert({ name, new DirectionalLight(    diffuse,
                                                                ambient,
                                                                specular,
                                                                direction   ) });
}

void Level::LoadPointLight(std::string name, XMFLOAT4 diffuse, XMFLOAT4 ambient, XMFLOAT4 specular, XMFLOAT3 position, XMFLOAT3 attenuation, float range)
{
    _pointLights->insert({ name, new PointLight(    diffuse,
                                                    ambient,
                                                    specular,
                                                    position,
                                                    attenuation,
                                                    range   ) });
}

void Level::LoadSpotLight(std::string name, XMFLOAT4 diffuse, XMFLOAT4 ambient, XMFLOAT4 specular, XMFLOAT3 position, XMFLOAT3 attenuation, float range, XMFLOAT3 direction, float spot)
{
    _spotLights->insert({ name, new SpotLight(  diffuse,
                                                ambient,
                                                specular,
                                                position,
                                                attenuation,
                                                range,
                                                direction,
                                                spot) });
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
    json textures = jFile["textures"];  //Gets the array
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

void Level::LoadDirectionalLights(json jFile)
{
    json lights = jFile["directionalLights"]; //Gets the array
    int size = lights.size();
    for (unsigned int i = 0; i < size; i++)
    {
        json lightDesc = lights.at(i);
        std::string name = lightDesc["name"];
        float diffuse_r = lightDesc["diffuse_r"];
        float diffuse_g = lightDesc["diffuse_g"];
        float diffuse_b = lightDesc["diffuse_b"];
        float diffuse_a = lightDesc["diffuse_a"];
        float ambient_r = lightDesc["ambient_r"];
        float ambient_g = lightDesc["ambient_g"];
        float ambient_b = lightDesc["ambient_b"];
        float ambient_a = lightDesc["ambient_a"];
        float specular_r = lightDesc["specular_r"];
        float specular_g = lightDesc["specular_g"];
        float specular_b = lightDesc["specular_b"];
        float specular_a = lightDesc["specular_a"];
        float direction_x = lightDesc["direction_x"];
        float direction_y = lightDesc["direction_y"];
        float direction_z = lightDesc["direction_z"];
        LoadDirectionalLight(name, XMFLOAT4(diffuse_r, diffuse_g, diffuse_b, diffuse_a), XMFLOAT4(ambient_r, ambient_g, ambient_b, ambient_a), XMFLOAT4(specular_r, specular_g, specular_b, specular_a), XMFLOAT3(direction_x, direction_y, direction_z));
    }
}

void Level::LoadPointLights(json jFile)
{
    json lights = jFile["pointLights"]; //Gets the array
    int size = lights.size();
    for (unsigned int i = 0; i < size; i++)
    {
        json lightDesc = lights.at(i);
        std::string name = lightDesc["name"];
        float diffuse_r = lightDesc["diffuse_r"];
        float diffuse_g = lightDesc["diffuse_g"];
        float diffuse_b = lightDesc["diffuse_b"];
        float diffuse_a = lightDesc["diffuse_a"];
        float ambient_r = lightDesc["ambient_r"];
        float ambient_g = lightDesc["ambient_g"];
        float ambient_b = lightDesc["ambient_b"];
        float ambient_a = lightDesc["ambient_a"];
        float specular_r = lightDesc["specular_r"];
        float specular_g = lightDesc["specular_g"];
        float specular_b = lightDesc["specular_b"];
        float specular_a = lightDesc["specular_a"];
        float position_x = lightDesc["position_x"];
        float position_y = lightDesc["position_y"];
        float position_z = lightDesc["position_z"];
        float attenuation_r = lightDesc["attenuation_r"];
        float attenuation_g = lightDesc["attenuation_g"];
        float attenuation_b = lightDesc["attenuation_b"];
        float range = lightDesc["range"];
        LoadPointLight(name, XMFLOAT4(diffuse_r, diffuse_g, diffuse_b, diffuse_a), XMFLOAT4(ambient_r, ambient_g, ambient_b, ambient_a), XMFLOAT4(specular_r, specular_g, specular_b, specular_a), XMFLOAT3(position_x, position_y, position_z), XMFLOAT3(attenuation_r, attenuation_g, attenuation_b), range);
    }
}

void Level::LoadSpotLights(json jFile)
{
    json lights;
    lights = jFile["spotLights"]; //Gets the array
    int size = lights.size();
    for (unsigned int i = 0; i < size; i++)
    {
        json lightDesc = lights.at(i);
        std::string name = lightDesc["name"];
        float diffuse_r = lightDesc["diffuse_r"];
        float diffuse_g = lightDesc["diffuse_g"];
        float diffuse_b = lightDesc["diffuse_b"];
        float diffuse_a = lightDesc["diffuse_a"];
        float ambient_r = lightDesc["ambient_r"];
        float ambient_g = lightDesc["ambient_g"];
        float ambient_b = lightDesc["ambient_b"];
        float ambient_a = lightDesc["ambient_a"];
        float specular_r = lightDesc["specular_r"];
        float specular_g = lightDesc["specular_g"];
        float specular_b = lightDesc["specular_b"];
        float specular_a = lightDesc["specular_a"];
        float position_x = lightDesc["position_x"];
        float position_y = lightDesc["position_y"];
        float position_z = lightDesc["position_z"];
        float attenuation_r = lightDesc["attenuation_r"];
        float attenuation_g = lightDesc["attenuation_g"];
        float attenuation_b = lightDesc["attenuation_b"];
        float range = lightDesc["range"];
        float direction_x = lightDesc["direction_x"];
        float direction_y = lightDesc["direction_y"];
        float direction_z = lightDesc["direction_z"];
        float spot = lightDesc["spot"];
        LoadSpotLight(name, XMFLOAT4(diffuse_r, diffuse_g, diffuse_b, diffuse_a), XMFLOAT4(ambient_r, ambient_g, ambient_b, ambient_a), XMFLOAT4(specular_r, specular_g, specular_b, specular_a), XMFLOAT3(position_x, position_y, position_z), XMFLOAT3(attenuation_r, attenuation_g, attenuation_b), range, XMFLOAT3(direction_x, direction_y, direction_z), spot);
    }
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

    std::ifstream fileOpen(path);

    fileOpen >> jFile;

    std::string levelName = jFile["name"];

    LoadMeshes(jFile);
    LoadMaterials(jFile);
    LoadTextures(jFile);
    LoadActors(jFile);
    LoadDirectionalLights(jFile);
    LoadPointLights(jFile);
    LoadSpotLights(jFile);

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