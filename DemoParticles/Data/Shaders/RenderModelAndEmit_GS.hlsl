#include "Globals.h"
#include "Noises/Random.h"

cbuffer modelToEmitConstantBuffer : register(b1)
{
    float2 scaleDensity;
    float2 offsetDensity;
    bool showModel;

    uint3 modelToEmitePadding;
}

struct GeometryShaderInput
{
    float4 Position : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
    float3 normal :TEXCOORD1;
    float4 worldPos: TEXCOORD2;
};

struct PixelShaderInput
{
    float4 Position : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD0;
    float3 normal : TEXCOORD1;
    float4 worldPos : TEXCOORD2;
    float unfoldFlag : TEXCOORD3;
};

void calculateUV(float3 v0, float3 v1, float3 v2, float3 triNormal, out float2 uv0, out float2 uv1, out float2 uv2)
{
    float3 positive_x = float3(1, 0, 0);
    float3 positive_y = float3(0, 1, 0);
    float3 nx = cross(positive_x, triNormal);
    if(length(nx) < 10e-5)
    {
        nx = positive_y; //choose another vector to cross with
    }
    nx = normalize(nx); //normalize the result of cross

    float3 ny = cross(nx, triNormal);
    ny = normalize(ny); //normalize the result of cross

    uv0.x = dot(v0, nx);
    uv0.y = dot(v0, ny);
    uv1.x = dot(v1, nx);
    uv1.y = dot(v1, ny);
    uv2.x = dot(v2, nx);
    uv2.y = dot(v2, ny);
}

[maxvertexcount(6)]
void main(triangle GeometryShaderInput input[3], inout TriangleStream<PixelShaderInput> OutStream, uint primID : SV_PrimitiveID)
{
    rng_state = wang_hash(rngSeed + primID);

    float3 faceNormal = normalize(cross(input[2].worldPos.xyz - input[0].worldPos.xyz, input[2].worldPos.xyz - input[1].worldPos.xyz));
    
    float2 uv[3];
    calculateUV(input[0].worldPos.xyz, input[1].worldPos.xyz, input[2].worldPos.xyz, faceNormal, uv[0], uv[1], uv[2]);

    //for(int i=0;i<3;++i)
    //so it's not backface culled (don't know why I should do that yet) PRobably the normal is calculated with wrong winding
    for(int i=2;i>=0;--i)
    {
        PixelShaderInput output;
        output.worldPos = input[i].worldPos;
        //output.Position = float4((input[i].Position.xy + offsetDensity) * scaleDensity, 0.0, 1.0);
        output.Position = float4((uv[i] + offsetDensity/* + primID*0.1*/) * scaleDensity, 0.0, 1.0);
        output.uv = input[i].uv;
        output.normal = faceNormal;
        output.color = input[i].color;
        output.unfoldFlag = 1;
        OutStream.Append(output);
    }
    
    OutStream.RestartStrip();
    
    if (showModel)
    {
        for (int i = 0; i < 3; ++i)
        {
            PixelShaderInput output;
            output.worldPos = input[i].worldPos;
            //output.Position = input[i].Position;
            output.Position = mul(float4(input[i].worldPos.xyz, 1.0), viewProj);
            output.uv = input[i].uv;
            output.normal = input[i].normal;
            output.color = input[i].color;
            output.unfoldFlag = 0;
            OutStream.Append(output);
        }
    }

    OutStream.RestartStrip();
}