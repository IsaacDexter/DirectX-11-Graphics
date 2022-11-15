//--------------------------------------------------------------------------------------
// File: DX11 Framework.hlsl
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
cbuffer ConstantBuffer : register( b0 )
{
	matrix World;  
	matrix View;
	matrix Projection;
    
    float4 DiffuseLight;
    float4 DiffuseMaterial;

    float4 AmbientLight;
    float4 AmbientMaterial;

    float3 DirectionToLight;
}

//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    //The normal's world space direction
    float4 NormalW : NORMAL0;
    float4 Color : COLOR0;
    //The world position
    float3 PosW : POSITION0;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS( float3 Pos : POSITION, float3 Normal : NORMAL)
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
    
    //output.Color = abs(output.NormalW);
    
    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( VS_OUTPUT input ) : SV_Target
{
    //Calculate diffuse lighting 
    float1 diffuseAmount;
    float4 totalPotentialDif;
    
    float1 dotAmount = dot(normalize(DirectionToLight.xyzz), input.NormalW);
    diffuseAmount = max(dotAmount, 0.0f);
    
    totalPotentialDif = float4(DiffuseMaterial.r * DiffuseLight.r, DiffuseMaterial.g * DiffuseLight.g, DiffuseMaterial.b * DiffuseLight.b, DiffuseMaterial.a * DiffuseLight.a);
    input.Color = diffuseAmount * totalPotentialDif;

    //Calculate ambient light
    float4 ambient = float4(AmbientMaterial.r * AmbientLight.r, AmbientMaterial.g * AmbientLight.g, AmbientMaterial.b * AmbientLight.b, AmbientMaterial.a * AmbientLight.a);
    input.Color += ambient;

    return input.Color;
}
