
cbuffer FrustrumCornersConstantBuffer : register(b0)
{
    float4 FrustumCorners[4];
};

struct VertexShaderInput
{
    float4 Position : POSITION;
    float2 uv : TEXCOORD0;
};

struct PixelShaderInput
{
    float4 Position : POSITION;
    float4 RayDir : TEXCOORD0;
    float2 uv : TEXCOORD1;
};

float4 FSQ_GetFrustumRay(in float2 texCoord)
{
    float index = texCoord.x + (texCoord.y * 2);
    return FrustumCorners[index];
}

PixelShaderInput main(VertexShaderInput input)
{
    PixelShaderInput output;

    output.Position = float4(input.Position.xyz, 1.0);

    /*float2 q = input.uv;
    float2 p = -1.0 + 2.0 * q;
    p *= resolution.x/resolution.y;

    float3 ww = normalize(cam_dir);
    float3 uu = normalize(cross( float3(0.0,-1.0,0.0), ww ));
    float3 vv = normalize(cross(ww,uu));
    output.uv = float4(normalize( p.x*uu + p.y*vv + 1.5*ww ), 1.0);
    */
    output.RayDir = FSQ_GetFrustumRay(input.uv);
    output.uv = input.uv;


    return output;
}
