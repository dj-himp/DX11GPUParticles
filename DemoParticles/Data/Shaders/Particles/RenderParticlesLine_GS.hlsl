#include "../Globals.h"
#include "ParticlesGlobals.h"

struct GeometryShaderInput
{
    float4 Position : SV_POSITION;
    float3 oPosition : TEXCOORD0;
    float4 Color : TEXCOORD1;
    float4 Normal : TEXCOORD2;
    float3 velocity : TEXCOORD3;
    uint orientation : TEXCOORD4;
    float size : TEXCOORD5;
};

struct PixelShaderInput
{
    float4 Position : SV_POSITION;
    float3 oPosition : TEXCOORD0;
    float4 Color : TEXCOORD1;
    float4 Normal : TEXCOORD2;
    float3 center : TEXCOORD3;
    float  radius : TEXCOORD4;
    float2 uv : TEXCOORD5;
};

[maxvertexcount(2)]
void main(point GeometryShaderInput input[1], inout LineStream<PixelShaderInput> OutStream)
{
    //discard if no particle
    if (input[0].Color.a <= 0.0)
        return;

    float3 pos = input[0].oPosition;
    float3 velocity = input[0].velocity;

    PixelShaderInput output;
    output.Color = input[0].Color;
    output.Normal = input[0].Normal;
    
    output.center = input[0].oPosition;
    output.radius = length(velocity * input[0].size);

    

    float4x4 viewProj = mul(view, proj);

    output.oPosition = pos;
    output.Position = mul(float4(output.oPosition, 1.0), viewProj);
    output.uv = float2(0.0, 1.0);
    OutStream.Append(output);

    output.oPosition = pos + velocity * input[0].size;
    output.Position = mul(float4(output.oPosition, 1.0), viewProj);
    output.uv = float2(0.0, 0.0);
    OutStream.Append(output);

    OutStream.RestartStrip();

}