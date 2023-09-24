#include "Globals.h"
#include "Noises/Random.h"

struct GeometryShaderInput
{
    float4 Position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 normal :TEXCOORD1;
    float4 worldPos: TEXCOORD2;
};

struct PixelShaderInput
{
    float4 Position : SV_POSITION;
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

    float ny = cross(nx, triNormal);
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

    float3 faceNormal = normalize(cross(input[2].worldPos - input[0].worldPos, input[2].worldPos - input[1].worldPos));
    //float3 faceNormal = normalize(cross(input[1].worldPos - input[0].worldPos, input[2].worldPos - input[0].worldPos));
    
    float2 uv[3];
    calculateUV(input[0].worldPos, input[1].worldPos, input[2].worldPos, faceNormal, uv[0], uv[1], uv[2]);


    float2 scaleDensity = float2(0.03, 0.03);
    float rnd = rand_xorshift_normalized();
    float2 offset = float2(rnd, rnd) * scaleDensity;

    //for(int i=0;i<3;++i)
    //so it's not backface culled (don't know why I should do that yet) PRobably the normal is calculated with wrong winding
    for(int i=2;i>=0;--i)
    {
        PixelShaderInput output;
        output.worldPos = input[i].worldPos;
        //output.Position = float4((input[i].Position.xy + offset) * scaleDensity, 0.0, 1.0);
        output.Position = float4((uv[i] + offset) * scaleDensity, 0.0, 1.0);
        output.uv = input[i].uv;
        output.normal = faceNormal;
        output.unfoldFlag = 1;
        OutStream.Append(output);
    }
    
    OutStream.RestartStrip();
    
    for(int i=0;i<3;++i)
    {
        PixelShaderInput output;
        output.worldPos = input[i].worldPos;
        //output.Position = input[i].Position;
        output.Position = mul(float4(input[i].worldPos.xyz, 1.0), viewProj);
        output.uv = input[i].uv;
        output.normal = input[i].normal;
        output.unfoldFlag = 0;
        OutStream.Append(output);
    }

    OutStream.RestartStrip();
}