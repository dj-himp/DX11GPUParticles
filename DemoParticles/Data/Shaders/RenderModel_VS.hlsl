
cbuffer ModelConstantBuffer : register(b0)
{
    float4x4 worldViewProj;
};

// Per-vertex data used as input to the vertex shader.
struct VertexShaderInput
{
    float3 Position : POSITION;
    float4 Color : COLOR;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 bitangent : BITANGENT;
    float2 texCoord : TEXCOORD;
};

// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
    float4 Position : SV_POSITION;
    float2 UVs : TEXCOORD0;
    float3 normal :TEXCOORD1;
};

// Simple shader to do vertex processing on the GPU.
PixelShaderInput main(VertexShaderInput input)
{
    PixelShaderInput output;

    output.Position = mul(float4(input.Position, 1.0), worldViewProj);
    //output.pos = mul(float4(input.Position.xyz, 1.0), worldViewProj);
    output.normal = input.normal;
    output.UVs = input.texCoord;

    return output;
}
