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

[maxvertexcount(4)]
void main(point GeometryShaderInput input[1], inout TriangleStream<PixelShaderInput> OutStream)
{
    //discard if no particle
    if (input[0].Color.a <= 0.0)
        return;

    PixelShaderInput output;
    output.Color = input[0].Color;
    output.Normal = input[0].Normal;

    float3 pos = input[0].oPosition;

    //TO DO : reduire particleSize plus la particule est proche de la cam

    //float particleSize = 0.0002f;
    float particleSize = 0.03;

    output.center = input[0].oPosition;
    output.radius = particleSize;

    float3 right;
    float3 up;
    
    if (input[0].orientation == PARTICLE_ORIENTATION_BILLBOARD)
    {
        //Camera Plane
        right = view._m00_m10_m20;
        up = view._m01_m11_m21;
    }
    else if (input[0].orientation == PARTICLE_ORIENTATION_BACKED_NORMAL)
    {
        //orient the particle to the normal;
        up = normalize(cross(input[0].Normal.xyz, float3(1.0, 0.0, 0.0)));
        right = normalize(cross(up, input[0].Normal.xyz));
    }
    else if (input[0].orientation == PARTICLE_ORIENTATION_DIRECTION)
    {
        //orient the particle perpendicullar to move direction;
        float3 direction = normalize(input[0].velocity);
        float3 newNormal = normalize(cross(direction, float3(0.0, 1.0, 0.0)));
        
        //invert normal to face camera
        newNormal *= dot(newNormal, normalize(-camDirection.xyz)) > 0.0 ? 1.0 : -1.0;

        //don't change rotation
        //up = normalize(cross(newNormal, float3(1.0, 0.0, 0.0)));

        //"rotate" the particle along it's direction
        up = normalize(cross(newNormal, direction));
        right = normalize(cross(up, newNormal)) * max(1.0, length(input[0].velocity) * 1.0);
        output.Normal = float4(newNormal, 0.0);

    }

    float4x4 viewProj = mul(view, proj);

    //in counterClockwise and right handed coordinate
    // 1    3
    // |  / |
    // | /  |
    // 2    4

    // Upper left vertex
    output.oPosition = pos + particleSize * (-right + up);
    output.Position = mul(float4(output.oPosition, 1.0), viewProj);
    output.uv = float2(0.0, 0.0);
    OutStream.Append(output);

    // Bottom left vertex
    output.oPosition = pos + particleSize * (-right - up);
    output.Position = mul(float4(output.oPosition, 1.0), viewProj);
    output.uv = float2(0.0, 1.0);
    OutStream.Append(output);

    // Upper right vertex
    output.oPosition = pos + particleSize * (right + up);
    output.Position = mul(float4(output.oPosition, 1.0), viewProj);
    output.uv = float2(1.0, 0.0);
    OutStream.Append(output);

    // Bottom right vertex
    output.oPosition = pos + particleSize * (right - up);
    output.Position = mul(float4(output.oPosition, 1.0), viewProj);
    output.uv = float2(1.0, 1.0);
    OutStream.Append(output);

    OutStream.RestartStrip();

}