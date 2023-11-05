
struct PixelShaderInput
{
    float4 Position : SV_POSITION;
    float2 UVs : TEXCOORD0;
    float3 normal :TEXCOORD1;
    float4 blendWeight : TEXCOORD2;
    nointerpolation uint4 boneIndices : TEXCOORD3;
};

struct PixelShaderOutput
{
    float4 Color : SV_Target;
};

PixelShaderOutput main(PixelShaderInput input)
{
    PixelShaderOutput output;

    output.Color = float4(normalize(input.normal).xyz, 1.0);
    //output.Color = float4(normalize(input.UVs).xy * 0.5f, 0.0, 1.0);
    //output.Color = float4(1.0, 0.0, 0.0, 1.0);

    //output.Color = diffuseTexture.Sample(LinearSampler, input.UVs);    

    //int boneID = 10;
    int boneID = 18;
    bool found = false;
    for(int i=0;i<4;++i)
    {
        if(input.boneIndices[i] == boneID)
        {
            if(input.blendWeight[i] >= 0.7)
                output.Color = float4(1.0, 0.0, 0.0, 1.0) * input.blendWeight[i];
            else if(input.blendWeight[i] >= 0.4 && input.blendWeight[i] <= 0.6)
                output.Color = float4(0.0, 1.0, 0.0, 1.0) * input.blendWeight[i];    
            else if(input.blendWeight[i] >= 0.1)
                output.Color = float4(1.0, 1.0, 0.0, 1.0) * input.blendWeight[i];    
            
            found = true;
            break;
        }
    }
    if(!found)
        output.Color = float4(0.0, 0.0, 1.0, 1.0);
    
    return output;
}
