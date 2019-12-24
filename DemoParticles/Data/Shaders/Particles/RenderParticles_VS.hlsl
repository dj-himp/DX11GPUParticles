
#include "ParticlesGlobals.h"

StructuredBuffer<Particle> particleList : register(t0);
StructuredBuffer<ParticleIndexElement> aliveParticlesIndex : register(t1);

struct GeometryShaderInput
{
    float4 Position : SV_POSITION;
    float3 oPosition : TEXCOORD0;
    float4 Color : TEXCOORD1;
    float4 Normal : TEXCOORD2;
    float3 velocity : TEXCOORD3;
    uint   orientation : TEXCOORD4;
    float  size : TEXCOORD5;
};

GeometryShaderInput main(uint vertexId : SV_VertexID)
{
    GeometryShaderInput output;
    
    uint index = aliveParticlesIndex[nbAliveParticles - vertexId - 1].index;
    
    Particle p = particleList[index];

    output.Position = p.position;
    output.oPosition = p.position.xyz;
    output.velocity = p.velocity.xyz;

    //float alpha = p.lifeSpan >= 0.0 ? saturate(p.age / p.lifeSpan) : 0.1;
    //float alpha = smoothstep(0.0, p.lifeSpan, p.age);
    float alpha = smoothstep(0.0, abs(p.lifeSpan), p.age);
    output.Color = p.color;
    output.Color.a *= alpha;
    output.Normal = p.normal;

    output.orientation = p.orientation;
    output.size = p.sizeStart + alpha * (p.sizeEnd - p.sizeStart);
    
    return output;
}