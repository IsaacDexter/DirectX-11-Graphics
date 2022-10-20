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
}

//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR0;
    //The world position
    float3 PosW : POSITION0;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT VS( float3 Pos : POSITION, float4 Color : COLOR )
{
    float4 pos4 = float4(Pos, 1.0f);
    
    VS_OUTPUT output = (VS_OUTPUT)0;
    //Shader handles the world positon
    output.Pos = mul( pos4, World);
    //Set the world position within output.
    output.PosW = output.Pos;
    output.Pos = mul( output.Pos, View );
    output.Pos = mul( output.Pos, Projection );
    //Light vertices near the bottom of the object darker using the y world position
    output.Color = mul(Color, output.PosW.y);
    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( VS_OUTPUT input ) : SV_Target
{
    //Firstly, find the Pixel Shader, named PS
    //1. We are going to modify the colour returned
    //2. Multiply the input.Color by a float4(0.5f, 0.5f, 0.5f, 1.0f);
    //3. Test and you should see your rendered objects dim in color
    //float4 ColorModifier = float4(0.5f, 0.5f, 0.5f, 1.0f);
    //input.Color = mul(input.Color, ColorModifier);
    
    return input.Color;
}
