#include "Level.h"

#pragma region Initialisation

Level::Level(char* path, ID3D11Device* d3dDevice, ID3D11DeviceContext* immediateContext, ID3D11Buffer* constantBuffer, XMFLOAT2 windowSize)
{
    m_d3dDevice = d3dDevice;
    m_immediateContext = immediateContext;
    m_constantBuffer = constantBuffer;
    m_windowSize = windowSize;

    Load(path);
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

void Level::LoadCamera(std::string name, std::string type, XMFLOAT4 eye, XMFLOAT4 at, XMFLOAT4 up, float windowWidth, float windowHeight, float nearDepth, float farDepth)
{
    if (type == "orbiting")
    {
        _cameras->insert({ name, new Camera(eye, at, up, windowWidth, windowHeight, nearDepth, farDepth) });
    }
    else if (type == "firstPerson")
    {
        _cameras->insert({ name, new FirstPersonCamera(eye, at, up, windowWidth, windowHeight, nearDepth, farDepth) });
    }
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

void Level::LoadCameras(json jFile)
{
    json cameras = jFile["cameras"];  //Gets the array
    int size = cameras.size();
    for (unsigned int i = 0; i < size; i++)
    {
        json cameraDesc = cameras.at(i);
        std::string name = cameraDesc["name"];    //Get the name
        std::string type = cameraDesc["type"];  //get the type, which is used to instanciate the correct class
        float eye_x = cameraDesc["eye_x"];
        float eye_y = cameraDesc["eye_y"];
        float eye_z = cameraDesc["eye_z"];
        float eye_w = cameraDesc["eye_w"];
        float at_x = cameraDesc["at_x"];
        float at_y = cameraDesc["at_y"];
        float at_z = cameraDesc["at_z"];
        float at_w = cameraDesc["at_w"];
        float up_x = cameraDesc["up_x"];
        float up_y = cameraDesc["up_y"];
        float up_z = cameraDesc["up_z"];
        float up_w = cameraDesc["up_w"];
        float nearDepth = cameraDesc["nearDepth"];
        float farDepth = cameraDesc["farDepth"];
        LoadCamera(name, type, XMFLOAT4(eye_x, eye_y, eye_z, eye_w), XMFLOAT4(at_x, at_y, at_z, at_w), XMFLOAT4(up_x, up_y, up_z, up_w), m_windowSize.x, m_windowSize.y, nearDepth, farDepth);    //Append this camera to its map
    }
    // set the current camera
    std::string defaultCamera = jFile["defaultCamera"];
    m_camera = _cameras->find(defaultCamera)->second;
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

    _actors = new std::map<std::string, Actor*>();
    _cameras = new std::map<std::string, Camera*>();

    _directionalLights = new std::map<std::string, DirectionalLight*>();
    _pointLights = new std::map<std::string, PointLight*>();
    _spotLights = new std::map<std::string, SpotLight*>();

    _night = false;
    if (_directionalLights->find("sun") != _directionalLights->end())
    {
        _night = _directionalLights->find("sun")->second->directionToLight.y < 0;
    }

    //Parse json
    json jFile;

    std::ifstream fileOpen(path);

    fileOpen >> jFile;

    std::string levelName = jFile["name"];

    LoadMeshes(jFile);
    LoadMaterials(jFile);
    LoadTextures(jFile);

    LoadActors(jFile);
    LoadCameras(jFile);

    LoadDirectionalLights(jFile);
    LoadPointLights(jFile);
    LoadSpotLights(jFile);

    // Initialize the world matrix
    XMStoreFloat4x4(&m_world, XMMatrixIdentity());
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

void Level::Update(float t, Keyboard::KeyboardStateTracker keys, Keyboard::State keyboard, Mouse::ButtonStateTracker mouseButtons, XMFLOAT2 mousePositon, Mouse::Mode mouseMode)
{
    // Animate actors
    _actors->find("cube")->second->SetRotation(XMFLOAT3(t / 2, t, 0.0f));
    _actors->find("cylinder")->second->SetRotation(XMFLOAT3(-t, -t / 2, 0.0f));
    _actors->find("barrel")->second->SetPosition(XMFLOAT3(t, 0.0f, 0.0f));

    //Move the skybox to the players position
    XMFLOAT4 cameraPos = m_camera->GetEye();
    _actors->find("skybox")->second->SetPosition(XMFLOAT3(cameraPos.x, cameraPos.y, cameraPos.z));

    /*if (_cameras->find("fixed3")->second == m_camera)
    {
        _cameras->find("fixed3")->second->LookAt(ToXMFLOAT4(_actors->find("barrel")->second->GetPosition()));
    }*/

    //Rotate billboards
    //For each billboard
    int i = 0;
    while (_actors->find("billboard" + std::to_string(i)) != _actors->end())
    {
        //Get the billboards postion
        XMVECTOR billboardPosition = XMLoadFloat3(&_actors->find("billboard" + std::to_string(i))->second->GetPosition());
        //The cameras position
        XMVECTOR cameraPosition = XMLoadFloat4(&cameraPos);
        //The billboards up (regular up as billboard does not turn in the x)
        XMVECTOR billboardUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
        //Get the rotation matrix by looking at the cameras position
        XMMATRIX r = XMMatrixLookAtLH(billboardPosition, cameraPosition, billboardUp);
        //store to a float 4x4 to access the values
        XMFLOAT4X4 rotation;
        XMStoreFloat4x4(&rotation, r);

        //Extract the roll and yaw from the rotation matrix
        float pitch = (float)asin(-rotation._23);
        float yaw = (float)atan2(rotation._13, rotation._33);
        float roll = (float)atan2(rotation._21, rotation._22);

        //rotate by these pitch roll and yaw values. This is so the m_rotation vector 3 doesnt lose track of itself
        _actors->find("billboard" + std::to_string(i))->second->SetRotation(XMFLOAT3(0.0f, yaw, roll));
        i++;
    }

    //Handle day and night cycle
    // If there is a sun
    if (_directionalLights->find("sun") != _directionalLights->end())
    {
        //Calculate its new angle from the time
        float sunAngle = sin(t / 5);
        _directionalLights->find("sun")->second->directionToLight.y = sunAngle; //Change the suns direction to this new angle

        if (sunAngle < 0 && !_night)    //if the sun has set
        {
            _night = true;  //Set it to night
            _actors->find("skybox")->second->SetTexture(_textures->find("nightDiffuse")->second);   //Set the skys texture to be a night sky
        }
        else if (sunAngle > 0 && _night) //if the sun has risen
        {
            _night = false; //Set it to day
            _actors->find("skybox")->second->SetTexture(_textures->find("dayDiffuse")->second); //Set the skys texture to be a day slky
        }
    }



    //Handle camera selection
    if (keys.pressed.D1)
    {
        m_camera = _cameras->find("fixed1")->second;
    }
    if (keys.pressed.D2)
    {
        m_camera = _cameras->find("fixed2")->second;
    }
    if (keys.pressed.D3)
    {
        m_camera = _cameras->find("fixed3")->second;
    }

    m_camera->Update(t, keys, keyboard, mouseButtons, mousePositon, mouseMode);

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

void Level::Draw()
{
    ConstantBuffer cb;

    XMMATRIX world = XMLoadFloat4x4(&m_world);
    XMMATRIX view = XMLoadFloat4x4(&m_camera->GetView());
    XMMATRIX projection = XMLoadFloat4x4(&m_camera->GetProjection()); 
    
    cb.mWorld = XMMatrixTranspose(world);
    cb.mView = XMMatrixTranspose(view);
    cb.mProjection = XMMatrixTranspose(projection);

    // Store lights in the constant buffer
    StoreDirectionalLights(&cb);
    StorePointLights(&cb);
    StoreSpotLights(&cb);

    cb.EyeWorldPos = m_camera->GetEye();

    DrawActors(&cb);
}

XMFLOAT4 Level::ToXMFLOAT4(XMFLOAT3 a, float w)
{
    return XMFLOAT4(a.x, a.y, a.z, w);
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