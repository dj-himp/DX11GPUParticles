#include "Globals.h"

cbuffer WorldConstantBuffer : register(b1)
{
    float4x4 world;
};

struct VertexShaderInput
{
    float4 Position : POSITION;
    float4 normal : NORMAL;
    float2 texCoord : TEXCOORD;
};

struct PixelShaderInput
{
    float4 Position : SV_POSITION;
    float4 pos : TEXCOORD0;
    float4 normal : TEXCOORD1;
    float2 uv : TEXCOORD2;
};

PixelShaderInput main(VertexShaderInput input)
{
    PixelShaderInput output;
       
    //output.Position = float4(2.0 * input.texCoord.x - 1.0, 1.0 - 2.0*input.texCoord.y, 0.0, 1.0);
    output.Position = float4(2.0 * input.texCoord.x - 1.0, 2.0 * input.texCoord.y - 1.0, 0.0, 1.0);
    output.pos = mul(float4(input.Position.xyz, 1.0), world);
    output.normal = mul(input.normal, world);
    output.uv = input.texCoord;

    return output;
}
