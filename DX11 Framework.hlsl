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
    
    float SpecularFalloff;  //4
    float3 Pad;             //16
};

cbuffer ConstantBuffer : register( b0 )
{
	matrix World;       //64
	matrix View;        //128
	matrix Projection;  //192
    
    //DirectionalLightBuffer directionalLights[2]; //64 * i
    
    float3 EyeWorldPos;     //12
    float pad;              //16
    
    //MaterialBuffer material;
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
    float4 Pos : SV_POSITION;   //16
    float4 Color : COLOR0;      //32
    
    float3 NormalW : NORMAL0;   //12
    float padNor;               //16
    
    float3 PosW : POSITION0;    //12
    float padPos;               //16
    
    float2 TexCoord : TEXCOORD0;//8
    float2 padTex;              //16
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
    output.PosW = (output.Pos).xyz;
    output.Pos = mul( output.Pos, View );
    output.Pos = mul( output.Pos, Projection );
    output.NormalW = normalize(mul(normal4, World)).xyz;
    output.TexCoord = TexCoord;
    
    //output.Color = abs(output.NormalW);
    
    return output;
}


//void CalculateDirectionalLight( DirectionalLightBuffer directionalLight, 
//                                MaterialBuffer material, 
//                                float3 normal, 
//                                float3 viewerDir, 
//                                float4 textureSpecular, 
//                                out float4 diffuse, 
//                                out float4 ambient, 
//                                out float4 specular )
//{
//    //Initialise output parameters
//    diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
//    ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
//    specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
//    //Calculate diffuse lighting 
//    float1 diffuseIntesity;
//    float4 diffusePotential;
    
//    float1 dotAmount = dot(normalize(directionalLight.directionToLight.xyz), normal);
//    diffuseIntesity = max(dotAmount, 0.0f);
    
//        //find the hadamard product of diffuse material and diffuse light. This is the maximum potential diffuse
//    diffusePotential = float4(material.DiffuseMaterial.r * directionalLight.diffuse.r, material.DiffuseMaterial.g * directionalLight.diffuse.g, material.DiffuseMaterial.b * directionalLight.diffuse.b, material.DiffuseMaterial.a * directionalLight.diffuse.a);
//    diffuse = diffuseIntesity * diffusePotential;

//        //Calculate ambient lighting
//        //find the hadamard product of ambient material and ambient light
//    ambient = float4(material.AmbientMaterial.r * directionalLight.ambient.r, material.AmbientMaterial.g * directionalLight.ambient.g, material.AmbientMaterial.b * directionalLight.ambient.b, material.AmbientMaterial.a * directionalLight.ambient.a);

//        // Calculate specular lighting
//    float1 specularIntensity;
//    float4 specularPotential;
    
//        // Calculate reflection direction
//    float3 reflectDir = normalize(reflect(directionalLight.directionToLight.xyz, normal));
//        //calculate viewer direction
    
//        // Find a power to raise the specular by from the shininess, a value stored within the specular maps .a, which is a value from 0-1 where 0 is least shiny and 1 is very shiny. The shinier an object, the smaller the highlight
//    float1 specularPower = textureSpecular.a * 25.0f;
//        // calculate specular intensity, using the specular falloff just calculated
//    specularIntensity = pow(max(dot(reflectDir, viewerDir), 0), specularPower);
//        //find the hadamard product of specular material and specular light, this is the maximum potential specular
//    specularPotential = float4(textureSpecular.r * directionalLight.specular.r, textureSpecular.g * directionalLight.specular.g, textureSpecular.b * directionalLight.specular.b, textureSpecular.a * directionalLight.specular.a);
//    specular = specularIntensity * specularPotential;

//    return;
//};

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_Target
{
    // Samples texture
    float4 textureColor = g_diffuseMap.Sample(SampLinear, input.TexCoord);
    //Samples specular map
    float4 textureSpecular = g_specularMap.Sample(SampLinear, input.TexCoord);
    
    //float3 viewerDir = normalize(input.PosW.xyz - EyeWorldPos);
    
    //Total light combined into a single color
    float4 lightColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    for (int i = 0; i < 2; i++)
    {
        ////CalculateDirectionalLight(directionalLights[i], material, input.NormalW, viewerDir, textureSpecular, diffuse, ambient, specular);
        ////lightColor += saturate(diffuse + ambient + specular);
        
        ////Initialise output parameters
        //diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
        //ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
        //specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
        ////Calculate diffuse lighting 
        //float1 diffuseIntesity;
        //float4 diffusePotential;
    
        //float1 dotAmount = dot(normalize(directionalLights[i].directionToLight.xyz), input.NormalW);
        //diffuseIntesity = max(dotAmount, 0.0f);
    
        ////find the hadamard product of diffuse material and diffuse light. This is the maximum potential diffuse
        //diffusePotential = float4(material.DiffuseMaterial.r * directionalLights[i].diffuse.r, material.DiffuseMaterial.g * directionalLights[i].diffuse.g, material.DiffuseMaterial.b * directionalLights[i].diffuse.b, material.DiffuseMaterial.a * directionalLights[i].diffuse.a);
        //diffuse = diffuseIntesity * diffusePotential;

        ////Calculate ambient lighting
        ////find the hadamard product of ambient material and ambient light
        //ambient = float4(material.AmbientMaterial.r * directionalLights[i].ambient.r, material.AmbientMaterial.g * directionalLights[i].ambient.g, material.AmbientMaterial.b * directionalLights[i].ambient.b, material.AmbientMaterial.a * directionalLights[i].ambient.a);

        //// Calculate specular lighting
        //float1 specularIntensity;
        //float4 specularPotential;
    
        //// Calculate reflection direction
        //float3 reflectDir = normalize(reflect(directionalLights[i].directionToLight.xyz, input.NormalW));
        ////calculate viewer direction
    
        //// Find a power to raise the specular by from the shininess, a value stored within the specular maps .a, which is a value from 0-1 where 0 is least shiny and 1 is very shiny. The shinier an object, the smaller the highlight
        //float1 specularPower = textureSpecular.a * 25.0f;
        //// calculate specular intensity, using the specular falloff just calculated
        //specularIntensity = pow(max(dot(reflectDir, viewerDir), 0), specularPower);
        ////find the hadamard product of specular material and specular light, this is the maximum potential specular
        //specularPotential = float4(textureSpecular.r * directionalLights[i].specular.r, textureSpecular.g * directionalLights[i].specular.g, textureSpecular.b * directionalLights[i].specular.b, textureSpecular.a * directionalLights[i].specular.a);
        //specular = specularIntensity * specularPotential;
        //lightColor += saturate(specular + ambient + diffuse);
    }

    //input.Color = textureColor * saturate(lightColor);
    
    return input.Color;
}
