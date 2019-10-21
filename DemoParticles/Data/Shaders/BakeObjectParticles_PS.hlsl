
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
    
    float3 derivX = ddx(input.pos.xyz);
    float3 derivY = ddy(input.pos.xyz);

    //output.Normal = length(derivX) > 0.01 ? input.normal = float4(1, 0, 0, 1) : float4(0, 1, 0, 1);
    
    //bool needMoreX = length(derivX) > 0.03;
    //bool needMoreY = length(derivY) > 0.03;

    //output.Normal = float4(needMoreX, 0.0, needMoreY, 1.0);

    output.Normal = input.normal;
    output.Normal.w = length(derivX) > 0.03 || length(derivY) > 0.03;

    return output;

}
