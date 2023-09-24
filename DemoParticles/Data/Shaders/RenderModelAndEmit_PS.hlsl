#include "Particles/ParticlesGlobals.h"


struct PixelShaderInput
{
    float4 Position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 normal : TEXCOORD1;
    float4 worldPos : TEXCOORD2;
    float unfoldFlag : TEXCOORD3;
};

struct PixelShaderOutput
{
    float4 Color : SV_Target;
};

AppendStructuredBuffer<Particle> particleBuffer : register(u2); //start at u2 because u0 & u1 are render target and depth see OMSetRenderTargetsAndUnorderedAccessViews
RWBuffer<uint> indirectDispatchArgs : register(u3);

//#define DEBUG_EMIT

PixelShaderOutput main(PixelShaderInput input)
{
    PixelShaderOutput output;
    
    if(input.unfoldFlag)
    {
#ifdef DEBUG_EMIT
        output.Color = float4(normalize(0.5 * input.normal + 0.5).xyz, 1.0);
#else
        Particle p = (Particle)0;
        p.position = input.worldPos;
        p.color = float4(normalize(0.5 * input.normal + 0.5).xyz, 1.0);
        p.normal = float4(input.normal, 1.0);
        particleBuffer.Append(p);

        InterlockedAdd(indirectDispatchArgs[0], 1);
        discard;
#endif
    }
    
    //discard;

    output.Color = float4(normalize(0.5 * input.normal + 0.5).xyz, 1.0);
    //output.Color = float4(normalize(input.UVs).xy * 0.5f, 0.0, 1.0);
    //output.Color = float4(1.0, 0.0, 0.0, 1.0);

    //output.Color = diffuseTexture.Sample(LinearSampler, input.UVs);    

    return output;
}
