
#include "ParticlesGlobals.h"

StructuredBuffer<Particle> particleList : register(t0);
StructuredBuffer<ParticleIndexElement> aliveParticlesIndex : register(t1);

struct PixelShaderInput
{
    float4 Position : SV_POSITION;
    float3 oPosition : TEXCOORD0;
    float4 Color : TEXCOORD1;
    float4 Normal : TEXCOORD2;
};

PixelShaderInput main(uint vertexId : SV_VertexID)
{
    PixelShaderInput output;
    
    uint index = aliveParticlesIndex[nbAliveParticles - vertexId - 1].index;
    
    Particle p = particleList[index];

    output.Position = p.position;
    output.oPosition = p.position.xyz;
    output.Color = float4(1.0, 0.0, 0.0, 1.0);
    output.Normal = float4(0.0, 0.0, 0.0, 1.0);

    return output;
}