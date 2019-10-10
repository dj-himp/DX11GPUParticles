cbuffer WorldConstantBuffer : register(b0)
{
    float4x4 world;
};

SamplerState pointSampler : register(s0);

Texture2D texPositions : register(t0);
Texture2D texNormals : register(t1);
Texture2D texColors : register(t2);

struct VertexShaderInput
{
    float2 uv : TEXCOORD0;
};

struct PixelShaderInput
{
    float4 Position : SV_POSITION;
    float3 oPosition : TEXCOORD0;
    float4 Color : TEXCOORD1;
    float4 Normal : TEXCOORD2;
};

PixelShaderInput main(VertexShaderInput input)
{
    PixelShaderInput output;
    
    float4 pos = texPositions.SampleLevel(pointSampler, input.uv, 0);
    float4 normal = texNormals.SampleLevel(pointSampler, input.uv, 0);
    float4 color = texColors.SampleLevel(pointSampler, input.uv, 0);
    
    output.Position = mul(float4(pos.xyz, 1.0), world);
    //output.Position = float4(pos.xy,1.0, 1.0);
    output.oPosition = output.Position.xyz;
    //TEMP pass the w of position to the color a to discard particle
    output.Color = float4(color.rgb, pos.w);
    //output.Normal = normalize( mul(float4(normal.xyz, 1.0), Local2World) );
    output.Normal = float4(normal.xyz, 1.0);
    
    //output.Normal.w = normal.w;

    return output;
}