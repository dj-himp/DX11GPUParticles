
SamplerState pointSampler : register(s0);
Texture2D tex : register(t0);

struct PixelShaderInput
{
    float4 Position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 color : TEXCOORD1;
};

struct PixelShaderOutput
{
    float4 color : SV_Target;
};

PixelShaderOutput main(PixelShaderInput input)
{
    PixelShaderOutput output;

    output.color = tex.Sample(pointSampler, input.uv).rgba;
    //output.color = float4(1, 0, 0, 1);
    //output.color = input.color;

    return output;
}
