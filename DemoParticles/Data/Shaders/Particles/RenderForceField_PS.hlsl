
struct PixelShaderInput
{
    float4 Position : SV_POSITION;
};

struct PixelShaderOutput
{
    float4 color : SV_Target;
};

PixelShaderOutput main(PixelShaderInput input)
{
    PixelShaderOutput output;
    
    output.color = float4(1.0, 0.0, 0.0, 1.0);

    return output;

}