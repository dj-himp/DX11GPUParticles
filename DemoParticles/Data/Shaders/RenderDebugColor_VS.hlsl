#include "Globals.h"

cbuffer WorldConstantBuffer : register(b1)
{
    float4x4 world;
};

struct VertexShaderInput
{
    float4 Position : POSITION;
    float4 color : COLOR;
};

struct PixelShaderInput
{
    float4 Position : SV_POSITION;
    float4 color : TEXCOORD0;    
};

// Simple shader to do vertex processing on the GPU.
PixelShaderInput main(VertexShaderInput input)
{
    PixelShaderInput output;

    output.Position = mul(float4(input.Position.xyz, 1.0), mul(world, viewProj));
    output.color = input.color;

    return output;
}