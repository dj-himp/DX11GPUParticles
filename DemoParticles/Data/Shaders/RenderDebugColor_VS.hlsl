cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
    float4x4 worldViewProj;
};

struct VertexShaderInput
{
    float3 Position : POSITION;
    float4 color : COLOR;
};

struct PixelShaderInput
{
    float4 Position : SV_POSITION;
    float4 color : TEXCOORD0;    
};

// Simple shader to do vertex processing on the GPU.
PixelShaderInput main(VertexShaderInput input)
{
    PixelShaderInput output;

    output.Position = mul(float4(input.Position, 1.0), worldViewProj);
    output.color = input.color;

    return output;
}