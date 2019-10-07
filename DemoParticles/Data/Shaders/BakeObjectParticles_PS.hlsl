
struct PixelShaderInput
{
    float4 Position : SV_POSITION;
    float4 pos : TEXCOORD0;
    float4 normal : TEXCOORD1;
    float2 UVs : TEXCOORD2;
};

struct PixelShaderOutput
{
    float4 Position : SV_Target0;
    float4 Normal : SV_Target1;
};

PixelShaderOutput main(PixelShaderInput input)
{
    PixelShaderOutput output;

    output.Position = input.pos;
    output.Position.w = 1.0; // input.UVs.x;
    
    //output.Normal = normalize(-input.normal);
    //output.Normal.w = input.UVs.y;
    output.Normal = float4(input.UVs, 0, 1);
    return output;

}
