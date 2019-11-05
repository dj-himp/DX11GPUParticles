#include "../Globals.h"

struct PixelShaderInput
{
    float4 Position : SV_POSITION;
    float3 oPosition : TEXCOORD0;
    float4 Color : TEXCOORD1;
    float4 Normal : TEXCOORD2;
    float3 center : TEXCOORD3;
    float  radius : TEXCOORD4;
};

struct PixelShaderOutput
{
    float4 color : SV_Target;
};

PixelShaderOutput main(PixelShaderInput input)
{
    PixelShaderOutput output;
    float3 normal = normalize(input.Normal.xyz);

    float3 ambient = sunColor.xyz * sceneAmbientPower;

    float NdotL = max(dot(normal, sunDirection.xyz), 0.0);
    float3 diffuse = NdotL * sunColor.xyz * sunColor.w;

    float3 viewDirection = normalize(camPosition.xyz - input.oPosition);
    float3 h = normalize(sunDirection.xyz + viewDirection);

    float NdotH = max(dot(normal, h), 0.0);
    float specIntensity = pow(saturate(NdotH), 32.0);
    float3 specular = specIntensity * sunSpecColor.xyz * sunSpecColor.w;

    output.color.xyz = ambient + diffuse + specular;// * input.Color.xyz;

    //output.color = 0.5 + normalize(input.Normal) * 0.5; //input.Color;
    //output.color = normalize(input.Normal); //input.Color;
    //output.color.a = 0.8;
    output.color.a = input.Color.a;

    float len = length(input.oPosition - input.center);
    output.color.a = output.color.a * (1.0 - smoothstep(0.0, input.radius, len));
    return output;

}