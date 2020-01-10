
cbuffer renderForceFieldConstantBuffer : register(b4)
{
    float3 size;
    float4x4 forceFieldVolume2World;

    uint simulatePadding;
}

Texture3D<float4> forceList : register(t0);
SamplerState PointClampSampler : register(s0);

struct GeometryShaderInput
{
    float3 oPosition : TEXCOORD0;
    float3 force : TEXCOORD1;
};

GeometryShaderInput main(uint vertexId : SV_VertexID)
{
    GeometryShaderInput output;
    
    //for the moment assume it's squared
    float cubeSize = size.x;
    float cubeSizeSquared = cubeSize * cubeSize;
    float3 uv;
    uv.z = floor(vertexId / cubeSizeSquared);
    uv.y = floor((vertexId - uv.z * cubeSizeSquared) / cubeSize);
    uv.x = vertexId - uv.z * cubeSizeSquared - uv.y * cubeSize;
    
    //output.force = forceList.SampleLevel(PointClampSampler, normalize(uv), 0).xyz;
    output.force = forceList.SampleLevel(PointClampSampler, uv / size.x, 0).xyz;
    
    output.oPosition = uv/size.x;// * 0.5;

    return output;
}