//--------------------------------------------------------------------------------------
// File: Tutorial02.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
struct VOut
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};
VOut VS(float4 position : POSITION, float4 color : COLOR)
{
    VOut output;

    output.position = position;
    output.color = color;

    return output;
}
//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
//float4 VS( float4 Pos : POSITION ) : SV_POSITION
//{
 //   return Pos;
//}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
//float4 PS( float4 Pos : SV_POSITION ) : SV_Target
//{
 //   return float4( 1.0f, 1.0f, 0.0f, 1.0f );    // Yellow, with Alpha = 1
//}
float4 PS(float4 position : SV_POSITION, float4 color : COLOR) : SV_TARGET
{
    return color;
}
