
struct PixelShaderInput
{
    float4 Position : SV_POSITION;
    float4 pos : TEXCOORD0;
    float4 normal : TEXCOORD1;
    float2 uv : TEXCOORD2;
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

    output.Normal = input.normal;
    
    //uncomment to add more particles on mesh (WIP)
    //float3 derivX = ddx(input.pos.xyz);
    //float3 derivY = ddy(input.pos.xyz);
    //output.Normal.w = length(derivX) > 0.03 || length(derivY) > 0.03;

    return output;

}
