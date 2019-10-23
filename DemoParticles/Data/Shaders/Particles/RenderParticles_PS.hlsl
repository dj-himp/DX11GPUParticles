struct PixelShaderInput
{
    float4 Position : SV_POSITION;
    float3 oPosition : TEXCOORD0;
    float4 Color : TEXCOORD1;
    float4 Normal : TEXCOORD2;
    float3 center : TEXCOORD3;
    float  radius : TEXCOORD4;
};

struct PixelShaderOutput
{
    float4 color : SV_Target;
};

PixelShaderOutput main(PixelShaderInput input)
{
    PixelShaderOutput output;

    output.color = 1.0 + normalize(input.Normal) * 0.5; //input.Color;
    //output.color = normalize(input.Normal); //input.Color;
    output.color.a = 0.8;

    float len = length(input.oPosition - input.center);
    output.color.a = output.color.a * (1.0 - smoothstep(0.0, input.radius, len));
    return output;

}