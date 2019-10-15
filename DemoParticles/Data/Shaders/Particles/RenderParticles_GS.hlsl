cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
    float4x4 world;
    float4x4 view;
    float4x4 proj;
};


struct PixelShaderInput
{
    float4 Position : SV_POSITION;
    float3 oPosition : TEXCOORD0;
    float4 Color : TEXCOORD1;
    float4 Normal : TEXCOORD2;
};

[maxvertexcount(4)]
void main(point PixelShaderInput input[1], inout TriangleStream<PixelShaderInput> OutStream)
{
    //discard if no particle
    if (input[0].Color.a <= 0.0)
        return;

    PixelShaderInput output;
    output.Color = input[0].Color;
    output.Normal = input[0].Normal;

    float3 pos = input[0].oPosition;

    //TO DO : reduire particleSize plus la particule est proche de la cam

    //float particleSize = 0.0002f;
    float particleSize = 0.01;

    //Camera Plane
    float3 right = view._m00_m10_m20;
    float3 up = view._m01_m11_m21;

    float4x4 viewProj = mul(view, proj);

    //in counterClockwise and right handed coordinate
    // 1    3
    // |  / |
    // | /  |
    // 2    4

    // Upper left vertex
    output.oPosition = pos + particleSize * (-right + up);
    output.Position = mul(float4(output.oPosition, 1.0), viewProj);
    OutStream.Append(output);

    // Bottom left vertex
    output.oPosition = pos + particleSize * (-right - up);
    output.Position = mul(float4(output.oPosition, 1.0), viewProj);
    OutStream.Append(output);

    // Upper right vertex
    output.oPosition = pos + particleSize * (right + up);
    output.Position = mul(float4(output.oPosition, 1.0), viewProj);
    OutStream.Append(output);

    // Bottom right vertex
    output.oPosition = pos + particleSize * (right - up);
    output.Position = mul(float4(output.oPosition, 1.0), viewProj);
    OutStream.Append(output);

    OutStream.RestartStrip();

}