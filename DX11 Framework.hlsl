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
VS_OUTPUT VS( float3 Pos : POSITION, float4 Normal : NORMAL)
{
    float4 pos4 = float4(Pos, 1.0f);
    float1 diffuseAmount;
    
    VS_OUTPUT output = (VS_OUTPUT)0;
    //Shader handles the world positon
    output.Pos = mul(pos4, World);
    //Set the world position within output.
    output.PosW = output.Pos;
    output.Pos = mul( output.Pos, View );
    output.Pos = mul( output.Pos, Projection );
    
    output.NormalW = normalize(mul(Normal, World));
    diffuseAmount = normalize(acos(dot(output.NormalW, float4(DirectionToLight, 0.0f))));
    output.Color = mul(diffuseAmount, mul(DiffuseLight, DiffuseMaterial));
    
    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( VS_OUTPUT input ) : SV_Target
{
    return input.Color;
}
