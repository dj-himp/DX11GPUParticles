
SamplerState PointSampler
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Clamp;
    AddressV = Clamp;
    AddressW = Clamp;
};

cbuffer ScreenQuadConstantBuffer : register(b0)
{
    float4x4 posScale;
};

Texture2D tex : register(t0);

// Per-vertex data used as input to the vertex shader.
struct VertexShaderInput
{
    float4 Position : POSITION;
    float2 uv : TEXCOORD0;
};

// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
    float4 Position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

// Simple shader to do vertex processing on the GPU.
PixelShaderInput main(VertexShaderInput input)
{
    PixelShaderInput output;

    output.Position = float4(mul(input.Position, posScale).xyz, 1.0);
    output.uv = input.uv;

    return output;
}
