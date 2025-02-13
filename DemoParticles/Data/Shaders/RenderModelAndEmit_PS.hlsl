#include "Particles/ParticlesGlobals.h"


struct PixelShaderInput
{
    float4 Position : SV_POSITION;
    float4 color : COLOR;
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

SamplerState TextureWrapSampler : register(s0);
Texture2D diffuseTex : register(t0);
Texture2D ilumTex : register(t1);

//#define DEBUG_EMIT

PixelShaderOutput main(PixelShaderInput input)
{
    PixelShaderOutput output;
    
    float4 diffuseColor = diffuseTex.Sample(TextureWrapSampler, input.uv);
    //float4 diffuseColor = input.color;
    //float4 ilumColor = ilumTex.Sample(TextureWrapSampler, input.uv);
    //diffuseColor += ilumColor;

    if(input.unfoldFlag)
    {
#ifdef DEBUG_EMIT
        output.Color = float4(normalize(0.5 * input.normal + 0.5).xyz, 1.0);
        return output;
#else
//        if(ilumColor.x > 0.3)
        {
            Particle p = (Particle)0;
            p.position = input.worldPos;
            p.color = diffuseColor;
            //p.color = input.color;//(normalize(0.5 * input.normal + 0.5).xyz, 1.0);
            p.normal = float4(input.normal, 1.0);
            particleBuffer.Append(p);

            InterlockedAdd(indirectDispatchArgs[0], 1);
        }
        discard;
#endif
    }
    
    //discard;

    output.Color = float4(normalize(0.5 * input.normal + 0.5).xyz, 1.0);
    //output.Color = diffuseColor;

    return output;
}
