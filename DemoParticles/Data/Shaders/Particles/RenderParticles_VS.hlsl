
#include "ParticlesGlobals.h"

StructuredBuffer<Particle> particleList : register(t0);
StructuredBuffer<ParticleIndexElement> aliveParticlesIndex : register(t1);

struct GeometryShaderInput
{
    float4 Position : SV_POSITION;
    float3 oPosition : TEXCOORD0;
    float4 Color : TEXCOORD1;
    float4 Normal : TEXCOORD2;
};

GeometryShaderInput main(uint vertexId : SV_VertexID)
{
    GeometryShaderInput output;
    
    uint index = aliveParticlesIndex[nbAliveParticles - vertexId - 1].index;
    
    Particle p = particleList[index];

    output.Position = p.position;
    output.oPosition = p.position.xyz;

    float alpha = p.lifeSpan >= 0.0 ? saturate(p.age / p.lifeSpan) : 0.1;
    output.Color = float4(1.0, alpha, 0.0, alpha);
    output.Normal = p.normal;

    return output;
}