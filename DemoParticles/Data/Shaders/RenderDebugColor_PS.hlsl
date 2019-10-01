
struct PixelShaderInput
{
    float4 Position : SV_POSITION;
    float4 color : TEXCOORD0;
};

struct PixelShaderOutput
{
    float4 color : SV_Target;
};

PixelShaderOutput main(PixelShaderInput input)
{
    PixelShaderOutput output;

    output.color = input.color;

    return output;
}
