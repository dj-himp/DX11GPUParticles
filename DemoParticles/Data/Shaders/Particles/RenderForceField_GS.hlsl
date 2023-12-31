#include "../Globals.h"
#include "ParticlesGlobals.h"

cbuffer renderForceFieldConstantBuffer : register(b4)
{
    float3 size;
    float4x4 forceFieldVolume2World;

    uint simulatePadding;
}

struct GeometryShaderInput
{
    float3 oPosition : TEXCOORD0;
    float3 force : TEXCOORD1;
};

struct PixelShaderInput
{
    float4 Position : SV_POSITION;
};

[maxvertexcount(5)]
void main(point GeometryShaderInput input[1], inout LineStream<PixelShaderInput> OutStream)
{
    
    PixelShaderInput output;
    
    float arrowLength = 0.05;
    float3 pos = input[0].oPosition;
    float3 dir = normalize(input[0].force);
    float4 end = float4(pos + dir * arrowLength, 1.0);

    float4x4 worldViewProj = mul(forceFieldVolume2World, viewProj);
    
    output.Position = mul(float4(pos, 1.0), worldViewProj);
    OutStream.Append(output);

    output.Position = mul(end, worldViewProj);
    OutStream.Append(output);

    float4 right = float4(cross(float3(0.0, 1.0, 0.0), dir), 1.0);
    output.Position = mul(float4(pos + dir * (arrowLength - 0.01), 1.0) + right * 0.002, worldViewProj);
    OutStream.Append(output);

    output.Position = mul(float4(pos + dir * (arrowLength - 0.01), 1.0) - right * 0.002, worldViewProj);
    OutStream.Append(output);

    output.Position = mul(end, worldViewProj);
    OutStream.Append(output);


    OutStream.RestartStrip();
}