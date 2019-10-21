struct PixelShaderInput
{
    float4 Position : SV_POSITION;
    float3 oPosition : TEXCOORD0;
    float4 Color : TEXCOORD1;
    float4 Normal : TEXCOORD2;
};

struct PixelShaderOutput
{
    float4 color : SV_Target;
};

PixelShaderOutput main(PixelShaderInput input)
{
    PixelShaderOutput output;

    //output.color = 1.0 + normalize(input.Normal) * 0.5; //input.Color;
    output.color = normalize(input.Normal); //input.Color;
    output.color.a = 0.1;
    return output;

}