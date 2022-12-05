//--------------------------------------------------------------------------------------
// File: DX11 Framework.hlsl
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------

// A struct containing all information pertaining to directional lights, to make the constant buffer better alligned
struct DirectionalLightBuffer
{
    float4  diffuse;            //16
    float4  ambient;            //32
    float4  specular;           //48
    float3  directionToLight;   //60
    float   pad;                //64
};

// A struct containing all information pertaining to point lights, to make the constant buffer better alligned
struct PointLightBuffer
{
    float4  diffuse;        //16
    float4  ambient;        //32
    float4  specular;       //48
    float3  position;       //60
    float   range;          //64
    float3  attenuation;    //76
    float   pad;            //80
};

// A struct containing all information pertaining to spot lights, to make the constant buffer better alligned
struct SpotLightBuffer
{
    float4  diffuse;        //16
    float4  ambient;        //32
    float4  specular;       //48
    float3  position;       //60
    float   range;          //64
    float3  attenuation;    //76
    float   pad;           //80
    float3  direction;      //92
    float   spot;            //96
};

struct MaterialBuffer
{
    float4 DiffuseMaterial; //16
    float4 AmbientMaterial; //32
    float4 SpecularMaterial; //48
    
    float specularFalloff;  //4
    float3 pad;             //16
};

cbuffer ConstantBuffer : register( b0 )
{
	matrix World;       //64
	matrix View;        //128
	matrix Projection;  //192

    MaterialBuffer material;
    
    DirectionalLightBuffer directionalLights[2]; //64 * i
    
    float4 EyeWorldPos;     //16
}
//--------------------------------------------------------------------------------------
// Texture Variables
//--------------------------------------------------------------------------------------
// Textures are assigned to a separate buffer inside our shader architecture, they are not included in the constant buffer with the other global variables we use. 
Texture2D g_diffuseMap : register(t0);
// define a second texture2d used to repersent the specular map, placed in the second register (t1)
Texture2D g_specularMap : register(t1);
// We also need to define a SamplerState to tell DirectX how to transform the texture data to fit into the correct size onscreen. 
SamplerState SampLinear : register(s0);


//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    //The normal's world space direction
    float4 NormalW : NORMAL0;
    float4 Color : COLOR0;
    //The world position
    float3 PosW : POSITION0;
    float2 TexCoord : TEXCOORD0;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS( float3 Pos : POSITION, float3 Normal : NORMAL, float2 TexCoord: TEXCOORD)
{
    float4 pos4 = float4(Pos, 1.0f);
    float4 normal4 = float4(Normal.xyz, 0.0f);
    
    VS_OUTPUT output = (VS_OUTPUT)0; 
    
    //Shader handles the world positon
    output.Pos = mul(pos4, World);
    //Set the world position within output.
    output.PosW = output.Pos;
    output.Pos = mul( output.Pos, View );
    output.Pos = mul( output.Pos, Projection );
    output.NormalW = normalize(mul(normal4, World));
    output.TexCoord = TexCoord;
    
    //output.Color = abs(output.NormalW);
    
    return output;
}

float4 Hadamard(float4 a, float4 b)
{
    return (float4(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w));
}

float3 Hadamard(float3 a, float3 b)
{
    return (float3(a.x * b.x, a.y * b.y, a.z * b.z));
}

float4 CalculateDiffuse(float4 diffuseMaterial, float4 diffuseLight, float1 incident)
{
    float1 diffuseIntesity;
    float4 diffusePotential;
    float4 diffuse;
    
    //Find the intesity of the diffuse light using the angle of incidence
    diffuseIntesity = max(incident, 0.0f);
    
    //find the hadamard product of diffuse material and diffuse light. This is the maximum potential diffuse
    diffusePotential = Hadamard(diffuseMaterial, diffuseLight);
    diffuse = saturate(diffuseIntesity * diffusePotential);
    
    return diffuse;
}

float4 CalculateAmbient(float4 ambientMaterial, float4 ambientLight)
{
    //Calculate ambient lighting
    //find the hadamard product of ambient material and ambient light
    float4 ambient = saturate(Hadamard(ambientMaterial, ambientLight));
    return ambient;
}

float4 CalculateSpecular(float4 specularMaterial, float4 specularLight, float1 specularAngle)
{
    // Calculate specular lighting
    float1 specularIntensity;
    float4 specularPotential;
    float4 specular;
    
    // Find a power to raise the specular by from the shininess, a value stored within the specular maps .a, which is a value from 0-1 where 0 is least shiny and 1 is very shiny. The shinier an object, the smaller the highlight
    float1 specularPower = specularMaterial.a * 25.0f;
    // calculate specular intensity, using the specular falloff just calculated
    specularIntensity = pow(max(specularAngle, 0), specularPower);
    //find the hadamard product of specular material and specular light, this is the maximum potential specular
    specularPotential = Hadamard(float4(specularMaterial.xyz, 1.0f), specularLight);
    specular = saturate(specularIntensity * specularPotential);
    return specular;
}

/// <summary>Calculates the ambient, diffuse and specular for an object being shone on by a directional light</summary>
/// <param name="ambMat">The ambient material of the object</param>
/// <param name="difMat">The diffuse material of the object</param>
/// <param name="speMat">The specular material of the object, the alpha value being the specular falloff</param>
/// <param name="difLig">The diffuse of the light</param>
/// <param name="ambLig">The ambient of the light</param>
/// <param name="speLig">The specular of the light</param>
/// <param name="dirLig">A direction float3 leading to the light</param>
/// <param name="norWor">A float4 world normal of this pixel</param>
/// <param name="dirEye">A direction vector leading to the eye</param>
/// <param name="ambOut">The return diffuse</param>
/// <param name="difOut">The return ambient</param>v
/// <param name="speOut">The return specular</param>
void CalculateDirectionalLighting
(
    float4 difMat,
    float4 ambMat,
    float4 speMat,
    float4 difLig,
    float4 ambLig,
    float4 speLig,
    float3 dirLig,
    float4 norWor,
    float4 dirEye,
    out float4 difOut,
    out float4 ambOut,
    out float4 speOut
)
{
    //Calculate diffuse lighting  
    float1 angleOfIncidence = dot(normalize(dirLig.xyzz), norWor);
    difOut = CalculateDiffuse(difMat, difLig, angleOfIncidence);
    
    //Calculate ambient lighting
    ambOut = CalculateAmbient(ambMat, ambLig);
    
    //Calculate specular Lighting
    float4 reflectDir = normalize(reflect(dirLig.xyzz, norWor));
    //A weirdly unnamed angle between the reflection direction and eye direction
    float1 angleBetweenReflectionAndEye = dot(reflectDir, dirEye);
    speOut = CalculateSpecular(speMat, speLig, angleBetweenReflectionAndEye);
    
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_Target
{
    // Samples texture
    float4 textureColor = g_diffuseMap.Sample(SampLinear, input.TexCoord);
    
    // Stores the specular falloff and material in a single float4
    float4 specularMaterial = float4(material.SpecularMaterial.xyz, material.specularFalloff);
    // If specular maps are being used
    if(true)
    {
        //Samples specular map and overwrites the material
        specularMaterial = g_specularMap.Sample(SampLinear, input.TexCoord);
    }
    
    float4 viewerDir = normalize(input.PosW.xyzz - EyeWorldPos);
    //Total light combined into a single color
    float4 lightColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    for (int i = 0; i < 2; i++)
    {
        float4 ambient;
        float4 diffuse;
        float4 specular;
        
        
        CalculateDirectionalLighting(material.DiffuseMaterial, material.AmbientMaterial, specularMaterial, directionalLights[i].diffuse, directionalLights[i].ambient, directionalLights[i].specular, directionalLights[i].directionToLight, input.NormalW, viewerDir, diffuse, ambient, specular);

        lightColor += saturate(specular + ambient + diffuse);
    }

    input.Color = textureColor * saturate(lightColor);
    
    return input.Color;
}
