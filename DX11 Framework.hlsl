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
    float   spot;           //80
    float3  direction;      //92
    float   pad;            //96
};

cbuffer ConstantBuffer : register( b0 )
{
	matrix World;       //64
	matrix View;        //128
	matrix Projection;  //192

    float4 DiffuseMaterial;     //16
    float4 AmbientMaterial;     //32
    float4 SpecularMaterial;    //48
    
    DirectionalLightBuffer directionalLight1; //64
    
    float4 EyeWorldPos;     //16
    float SpecularFalloff;  //20
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


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(VS_OUTPUT input) : SV_Target
{
    // Samples texture
    float4 textureColor = g_diffuseMap.Sample(SampLinear, input.TexCoord);
    //Samples specular map
    float4 textureSpecular = g_specularMap.Sample(SampLinear, input.TexCoord);
    
    //Calculate diffuse lighting 
    float1 diffuseIntesity;
    float4 diffusePotential;
    float4 diffuse;
    
    float1 dotAmount = dot(normalize(directionalLight1.directionToLight.xyzz), input.NormalW);
    diffuseIntesity = max(dotAmount, 0.0f);
    
    //find the hadamard product of diffuse material and diffuse light. This is the maximum potential diffuse
    diffusePotential = float4(DiffuseMaterial.r * directionalLight1.diffuse.r, DiffuseMaterial.g * directionalLight1.diffuse.g, DiffuseMaterial.b * directionalLight1.diffuse.b, DiffuseMaterial.a * directionalLight1.diffuse.a);
    diffuse = diffuseIntesity * diffusePotential;

    //Calculate ambient lighting
    //find the hadamard product of ambient material and ambient light
    float4 ambient = float4(AmbientMaterial.r * directionalLight1.ambient.r, AmbientMaterial.g * directionalLight1.ambient.g, AmbientMaterial.b * directionalLight1.ambient.b, AmbientMaterial.a * directionalLight1.ambient.a);

    // Calculate specular lighting
    float1 specularIntensity;
    float4 specularPotential;
    float4 specular;
    
    // Calculate reflection direction
    float4 reflectDir = normalize(reflect(directionalLight1.directionToLight.xyzz, input.NormalW));
    //calculate viewer direction
    float4 viewerDir = normalize(input.PosW.xyzz - EyeWorldPos);
    // Find a power to raise the specular by from the shininess, a value stored within the specular maps .a, which is a value from 0-1 where 0 is least shiny and 1 is very shiny. The shinier an object, the smaller the highlight
    float1 specularPower = textureSpecular.a * 25.0f;
    // calculate specular intensity, using the specular falloff just calculated
    specularIntensity = pow(max(dot(reflectDir, viewerDir), 0), specularPower);
    //find the hadamard product of specular material and specular light, this is the maximum potential specular
    specularPotential = float4(textureSpecular.r * directionalLight1.specular.r, textureSpecular.g * directionalLight1.specular.g, textureSpecular.b * directionalLight1.specular.b, textureSpecular.a * directionalLight1.specular.a);
    specular = specularIntensity * specularPotential;

    input.Color = textureColor + specular + ambient + diffuse;

    
    
    return input.Color;
}
