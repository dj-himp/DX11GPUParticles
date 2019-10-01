
struct PixelShaderInput
{
    float4 Position : SV_POSITION;
    float2 UVs : TEXCOORD0;
    float3 normal :TEXCOORD1;
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

    return output;
}
