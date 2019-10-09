/*#define PackParticlesRootSignature  "UAV(u0), " \
                                    "UAV(u1), " \
                                    "UAV(u2), " \
                                    "SRV(t0), " \
                                    //"SRV(t1)"
*/

RWTexture2D<float4> texOutput : register(u0);
RWTexture2D<float4> texOutput2 : register(u1);
RWStructuredBuffer<uint> counterBuffer : register(u2);

Texture2D<float4> srcTexture : register(t0);
Texture2D<float4> srcTexture2 : register(t1);

//[RootSignature(PackParticlesRootSignature)]
[numthreads(32, 32, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    float4 srcPixel = srcTexture[id.xy].rgba;
    float4 srcPixel2 = srcTexture2[id.xy].rgba;
    if (srcPixel.a > 0.0)
    {
        uint oldValue;
        InterlockedAdd(counterBuffer[0], 1, oldValue);
        
        uint y = oldValue / 1024;
        uint x = oldValue % 1024;
        uint2 idx = uint2(x, y);

        texOutput[idx] = /*float4(oldValue / 737280.0, 0.2, 0.0, 1.0);*/ srcPixel;
        texOutput2[idx] = srcPixel2;
    }
}
