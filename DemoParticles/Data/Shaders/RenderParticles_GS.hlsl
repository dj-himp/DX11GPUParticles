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
    //output.Color = float4(input[0].Normal.x, input[0].Normal.y, input[0].Normal.z, 1.0);
    output.Color = input[0].Color;
    //output.Color = float4(1.0, 0.0, 0.0, 1.0);
    output.Normal = input[0].Normal;

    float3 pos = input[0].oPosition;

    //pos = float3(0.0, 0.0, -3.0);

    //TO DO : reduire particleSize plus la particule est proche de la cam

    float particleSize = 0.001f;
    //float particleSize = 0.002f;

    //Camera Plane
    float3 right = view._m00_m10_m20;
    float3 up = view._m01_m11_m21;

 
    //if(dot(Up, output.Normal) != 0)
    {
        //orient the particle to face the normal
        //Right = normalize(cross( Up, output.Normal.xyz));
        //Up = normalize(cross(output.Normal.xyz, Right));
    }
    /*else
    {
        Right = normalize(cross(output.Normal.xyz, Right));	 //Right is At but the Right in the cross is the real right	
        Up = normalize(cross(Right, output.Normal.xyz));
    }*/

    /*float3 Right2 = normalize(cross( Up, output.Normal.xyz));
    float3 Up2 = normalize(cross(output.Normal.xyz, Right));
    output.Color = float4(0.5 * normalize(Right2 + Up2) + 0.5, 1.0);*/

    /*float3 At = normalize(cross( Right, output.Normal.xyz));
    float3 Right2 = normalize(cross(output.Normal.xyz, At));
    //output.Color = float4(0.5 * normalize(Right2 + Up2) + 0.5, 1.0);
    //output.Color = float4(0.5 * normalize(At) + 1.0, 1.0);
    Right = Right2;
    Up = At;*/

    float4x4 viewProj = mul(view, proj);

    // Upper left vertex
    //output.Color = float4(0.0, 1.0, 0.0, 1.0);
    //output.Color = float4(-Right+Up, 1.0);
    output.oPosition = pos + particleSize * (-right + up);
    output.Position = mul(float4(output.oPosition, 1.0), viewProj);
    OutStream.Append(output);

    // Upper right vertex
    //output.Color = float4(1.0, 1.0, 0.0, 1.0);
    //output.Color = float4(Right+Up, 1.0);
    output.oPosition = pos + particleSize * (right + up);
    output.Position = mul(float4(output.oPosition, 1.0), viewProj);
    OutStream.Append(output);

    // Bottom left vertex
    //output.Color = float4(0.0, 0.0, 0.0, 1.0);
    //output.Color = float4(-Right-Up, 1.0);
    output.oPosition = pos + particleSize * (-right - up);
    output.Position = mul(float4(output.oPosition, 1.0), viewProj);
    OutStream.Append(output);

    // Bottom right vertex
    //output.Color = float4(1.0, 0.0, 0.0, 1.0);
    //output.Color = float4(Right-Up, 1.0);
    output.oPosition = pos + particleSize * (right - up);
    output.Position = mul(float4(output.oPosition, 1.0), viewProj);
    OutStream.Append(output);

    OutStream.RestartStrip();

}