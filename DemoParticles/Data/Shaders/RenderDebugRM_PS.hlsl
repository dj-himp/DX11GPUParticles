#include "Globals.h"

struct PixelShaderInput
{
    float4 Position : SV_POSITION;
    float4 rayDir : TEXCOORD0;
    float2 uv : TEXCOORD1;
};

struct PixelShaderOutput
{
    float4 color : SV_Target;
};

//----------------------------

struct Ray
{
    float3 origin;
    float3 direction;
};

Ray generateRay(PixelShaderInput input)
{
    Ray r;

    r.origin = camPosition;
    r.direction = normalize(input.rayDir.xyz);

    return r;
}

float2 map(float3 pos)
{
    float sphereRadius = 1.0f;

    float d = length(pos) - sphereRadius;

    return float2(d, 1.0);
}

//raymarching, p - position/origin, d - direction, s - stepping multiplier (0.25-1.0), t - threshold, n maximum number of steps
//returns a floattor float3(steps/(n-1), stepped length, last stepped length) 
float4 rm(float3 p, float3 d, float s, float t, float n) {
    float3 q = p;
    float l, i;
    float2 obj;
    for (i = 0.; i < 1.; i += 1.0 / n) {
        obj = map(p);
        l = abs(obj.x);
        p += l * d*s;
        if (l < t) return float4(i, length(q - p), l, obj.y);

    };
    return float4(1.0, -1.0, -1.0, -1.0);
}

PixelShaderOutput main(PixelShaderInput input)
{
    PixelShaderOutput output;

    Ray ray = generateRay(input);
    
    output.color = float4(1.3, 1.3, 1.3, 1.0);

    float4 obj = rm(ray.origin, ray.direction, 1.0, 0.001, 200.0);

    if (obj.w > 0.0)
    {
        output.color = float4(1, 0, 0, 1);
    }
    

    //output.color = float4(normalize(input.rayDir).xyz, 1.0);

    return output;
}
