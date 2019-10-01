// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
	float4 Position : SV_POSITION;
	float2 uv : TEXCOORD0;
};

struct PixelShaderOutput
{
    float4 color : SV_Target;
};

// A pass-through function for the (interpolated) color data.
PixelShaderOutput main(PixelShaderInput input)
{
    PixelShaderOutput output;

    //output.color = tex.Sample(PointSampler,input.uv).rgba;
    //output.color = float4(tex.Sample(PointSampler,input.uv).rgb, 1.0);
    output.color = float4(input.uv, 0.0, 1.0);
    //output.color = float4(1.0, 0.0, 0.0, 1.0);

    return output;
}
