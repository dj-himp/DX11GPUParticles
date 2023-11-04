#include "Globals.h"
#include "Particles/ParticlesGlobals.h"

struct PixelShaderInput
{
    float4 Position : SV_POSITION;
    float4 RayDir : TEXCOORD0;
    float2 uv : TEXCOORD1;
};



struct Ray
{
    float3 origin;
    float3 direction;
};

struct Light
{
    float3 position; //ou direction
    float3 color;
};

struct Plane
{
    float3 normal;
    float height;
};

static Light lig1;
static Plane plan1;
static Plane plan2;
static Plane plan3;
static Plane plan4;

static float3 ambColor = float3(0.1, 0.15, 0.2);

float mod(float x, float y)
{
    return x - y * floor(x / y);
}
float3 mod(float3 x, float y)
{
    return x - y * floor(x / y);
}
float3 mod(float3 x, float3 y)
{
    return x - y * floor(x / y);
}

void initScene()
{

    lig1.position = camPosition.xyz;// + float3(-0.2,0.2,-2); //point light
    //lig1.position = float3(0.0,0.0,1.0); //directionnal light
    lig1.color = float3(1, 1, 1);

    plan1.normal = normalize(float3(0.0, -1.0, 0.0));
    plan1.height = 0.35;

    plan2.normal = normalize(float3(0.0, 1.0, 0.0));
    plan2.height = 0.35;

    plan3.normal = normalize(float3(1.0, 0.0, 0.0));
    plan3.height = 0.35;

    plan4.normal = normalize(float3(-1.0, 0.0, 0.0));
    plan4.height = 0.35;
}

Ray generateRay(PixelShaderInput input)
{
    Ray r;

    //camPosition += time * float3(0.0, 0.0, -1.0);
    r.origin = camPosition.xyz;
    r.direction = normalize(input.RayDir.xyz);

    return r;
}


float3 objRepeat(float3 pos, float3 c)
{
    float3 q = mod(pos, c);// - 0.5 * c;			
    return q;
}


float2 MyMin(float2 a, float2 b)
{
    if (a.x <= b.x)
        return a;
    return b;
}

float2 MyMax(float2 a, float2 b)
{
    if (a.x >= b.x)
        return a;
    return b;
}

float maxcomp(in float3 p)
{
    return max(p.x, max(p.y, p.z));
}

float rand(float2 coordinate) {
    return frac(sin(dot(coordinate.xy, float2(12.9898, 78.233))) * 43758.5453);
}

float simpleRibbon(float3 pos)
{
    float delay = 3.0;
    float w = 0.42 * max(sin(pos.x), 0.000);
    float h = 0.002;// + (1.0 + sin(pos.z)) * 0.5;
    float3 shape = float3(cos(pos.z * 1.5)*0.03, 0.2 - cos(pos.z * 1.5) * 0.02, 0.0);
    return length(max(abs(pos - shape) - float3(w, h, time + delay), float3(0.0, 0.0, 0.0)));

    //float3 shape = float3(0.0, cos(pos.x * 1.5) * 0.5, 0.0);
    //return length(max(abs(pos - shape) - float3(time + delay , h, w), float3(0.0)));
}

//at origin
float distanceBox(float3 pos, float3 boxSize)
{
    //signed
    float3  di = abs(pos) - boxSize;
    float mc = maxcomp(di);
    return min(mc, length(max(di, 0.0)));

    //unsigned
    //return length(max(abs(pos)-boxSize, 0.0));
}

float distanceMengerSponge(float3 pos)
{

    float d = distanceBox(pos, float3(2.0, 2.0, 2.0));
    float4 res = float4(d, 1.0, 0.0, 0.0);

    float s = 1.0;
    for (int m = 0; m < 4; m++)
    {
        float3 a = mod(pos*s, 2.0) - 1.0;
        s *= 3.0;
        float3 r = abs(1.0 - 3.0*abs(a));

        float da = max(r.x, r.y);
        float db = max(r.y, r.z);
        float dc = max(r.z, r.x);
        float c = (min(da, min(db, dc)) - 1.0) / s;

        if (c > d)
        {
            d = c;
            res = float4(d, 0.2*da*db*dc, (1.0 + float(m)) / 4.0, 0.0);
        }
    }

    return res.x;
}

float distancePlane(float3 pos, Plane plan)
{
    //plan
    return dot(pos, plan.normal) + plan.height;
}

float displacement(float3 pos)
{
    return 0.01 * sin(20.0*pos.x) * sin(20.0*pos.y) * sin(20.0*pos.z);
}

float3 Twist(float3 p)
{
    float twistCoef = 0.02 * sin(time);
    float c = cos(twistCoef*p.y);
    float s = sin(twistCoef*p.y);
    float2x2  m = float2x2(c, -s, s, c);
    float3 q = float3(mul(m, p.xz), p.y);
    return q;
}

/*float Blend( float d1, float d2 )
{
    float dd = smoothcurve(d1-d2);
    return lerp(d1,d2,dd);
}*/

float distanceMetaBallsOnly(float3 pos)
{
    float e = 0.01;
    float magicNumber = 0.04;

    float3 sphPos1 = float3(0.175 * sin(time), 0.25 * cos(time), -1.0) + camPosition.xyz;
    float sph1Radius = 0.003;

    float3 sphPos2 = float3(-0.1750 * cos(time) * sin(time), 0.25 * sin(time), -1.0) + camPosition.xyz;
    float sph2Radius = 0.004;

    float3 sphPos3 = float3(0., 0.25 * sin(time) * cos(time), -1.0 + 0.175 * cos(time) * sin(time)) + camPosition.xyz;
    float sph3Radius = 0.0003;

    float d1 = length(sphPos1 - pos) - sph1Radius;
    float d2 = length(sphPos2 - pos) - sph2Radius;
    float d3 = length(sphPos3 - pos) - sph3Radius;

    float srf = 1.0 / (d1 + sph1Radius + e) + 1.0 / (d2 + sph2Radius + e) + 1.0 / (d3 + sph3Radius + e);
    float dsrf = 1.0 / srf - magicNumber;

    return dsrf;
}

float distanceMetaBalls(float3 pos, float distobj1)
{
    float e = 0.01;
    float magicNumber = 0.04;

    float3 sphPos1 = float3(0.175 * sin(time), 0.25 * cos(time), -1.0) + camPosition.xyz;
    float sph1Radius = 0.003;

    float3 sphPos2 = float3(-0.1750 * cos(time) * sin(time), 0.25 * sin(time), -1.0) + camPosition.xyz;
    float sph2Radius = 0.004;

    float3 sphPos3 = float3(0., 0.25 * sin(time) * cos(time), -1.0 + 0.175 * cos(time) * sin(time)) + camPosition.xyz;
    float sph3Radius = 0.0003;

    float d1 = length(sphPos1 - pos) - sph1Radius;
    float d2 = length(sphPos2 - pos) - sph2Radius;
    float d3 = length(sphPos3 - pos) - sph3Radius;

    float srf = 1.0 / (d1 + sph1Radius + e) + 1.0 / (d2 + sph2Radius + e) + 1.0 / (d3 + sph3Radius + e) + 1.0 / (distobj1 + 0.125 + e);
    float dsrf = 1.0 / srf - magicNumber;

    return dsrf;
}

float2 map(float3 pos)
{
    float2 d;

    //float2 d1 = float2(simpleRibbon(pos), 1.0);
    //float2 d7 = float2(distanceMetaBalls(pos, d1.x), 1.0);

    float2 d2 = float2(distanceMengerSponge(objRepeat(pos, float3(2.0, 2.0, 2.0))), 2.0);

    float2 d4 = float2(distancePlane(pos, plan2), 3.0);

    float2 d7 = float2(distanceMetaBallsOnly(pos), 1.0);

    //d = MyMin(d1, d7);

    //d = MyMin(d1, MyMin(d2, d4));
    //d = MyMin(d1, MyMin(d2, MyMin(d4, d7))); //ribbon, meta, plan, menger
    //d = MyMin(d1, MyMin(d4, d7));

    d = MyMin(d2, MyMin(d4, d7)); //meta, plan, menger

    return d;
}


float3 calcNormal(float3 pos)
{
    float3 eps = float3(.0001, 0.0, 0.0);
    float3 nor;
    nor.x = map(pos + eps.xyy).x - map(pos - eps.xyy).x;
    nor.y = map(pos + eps.yxy).x - map(pos - eps.yxy).x;
    nor.z = map(pos + eps.yyx).x - map(pos - eps.yyx).x;

    return normalize(nor);
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

float softShadow(float3 pos, float3 ldir, float minT, float k, float3 lPos)
{
    float res = 1.0;
    float dt = 0.1;
    float t = minT;

    float3 lfloat = lPos - pos;
    float sqrDistLight = length(lfloat);

    int maxNbStep = 200;
    for (int i = 0; i < maxNbStep; i++)
    {
        float h = map(pos + t * ldir).x;
        if (h < 0.002)
            return 0.3;
        //res = min(res, k*h/t);
        t += h;

        if (t >= sqrDistLight - 0.006)
            return res;
    }
    return res;
}

float Attenuation(float distance, float range, float a, float b, float c)
{
    float Atten = 1.0f / (a * distance * distance + b * distance + c);

    // Use the step() intrinsic to clamp light to 
    // zero out of its defined range
    return step(distance, range) * clamp(Atten, 0.0, 1.0);
}

float3 lighting(float3 pos, float3 norm, float3 rayDir, out float specInt, out float sh, out float atten)
{

    float3 ldir = -normalize(pos - lig1.position); //point light
    //float3 ldir = normalize(lig1.position); //directionnal light

    float NL = max(dot(norm, ldir), 0.0);
    float3 dif = NL * lig1.color;

    float3 ref = reflect(rayDir, norm);
    specInt = pow(clamp(dot(ldir, ref), 0.0, 1.0), 16.0);

    sh = 1.0;//softShadow(pos, ldir, 0.006, 18.0, lig1.position);

    //atten = Attenuation(length(pos - lig1.position), 5.0, 1.0, 0.003, 0.001);
    atten = pow(max(dot(-camDirection.xyz, ldir), 0), 32);

    return dif * atten * float3(sh, (sh + sh * sh)*0.5, sh*sh);
}

void applyScattering(in out float3 col, float dist)
{
    //exponential decay
    float f = pow(2.0, -0.40*dist);
    //extinction
    col *= f;
    //in-scattering
    col.x += 0.50*(1.0 - f);
    col.y += 0.55*(1.0 - f);
    col.z += 0.60*(1.0 - f);
}

float3 ColorMaterial1(float3 pos, float3 normal, float iteration, float3 eyeDir)
{
    //float3 baseColor = float3(0.9, 0.1, 0.1);
    //float3 baseColor = float3(0.0, 0.25, 0.5);
    float3 baseColor = float3(0.5, 0.25, 0.25);

    float specInt;
    float sh;
    float atten;
    float3 dif = lighting(pos, normal, eyeDir, specInt, sh, atten);


    float ambInt = 0.5 + 0.5*normal.y;
    float3 amb = ambInt * ambColor;

    float3 color = 1.0 * dif;
    color += 0.2 * amb;

    color *= baseColor;
    //ao
    //float ao = 0.5 + 0.5*normal.y;
    //color *= 1.2 * ao;

    float k = 0.4;
    for (float i = 0.0; i < 5.0; ++i)
    {
        float colTemp = i * k - map(pos + normal * i * k).x;
        color -= float3(colTemp, colTemp, colTemp) / pow(2.0, i);
    }

    //specular
    color += pow(specInt, 8.0) * lig1.color * atten * sh;//float3(sh, (sh+sh*sh)*0.5, sh*sh );

    color += baseColor;

    return color;
}

float3 ColorMaterial2(float3 pos, float3 normal, float iteration, float3 eyeDir)
{
    //float3 baseColor = float3(0.7,1.0,0.7);
    float3 baseColor = float3(1, 1, 1);
    float specInt;
    float sh;
    float atten;
    float3 dif = lighting(pos, normal, eyeDir, specInt, sh, atten);


    float ambInt = 0.5 + 0.5*normal.y;
    float3 amb = ambInt * ambColor;

    float3 color = 1.0 * dif;
    color += 0.2 * amb;

    color *= baseColor;
    //ao
    //float ao = 0.5 + 0.5*normal.y;
    //color *= 1.2 * ao;

    float k = 0.02;
    for (float i = 0.0; i < 5.0; ++i)
    {
        float colTemp = i * k - map(pos + normal * i * k).x;
        color -= float3(colTemp, colTemp, colTemp) / pow(2.0, i);
    }

    //specular
    color += pow(specInt, 8.0) * lig1.color * atten * sh;//float3(sh, (sh+sh*sh)*0.5, sh*sh );

    return color;
}

float3 ColorMaterial3(float3 pos, float3 normal, float iteration, float3 eyeDir)
{
    float3 baseColor = float3(0.4, 0.6, 0.9);
    float specInt;
    float sh;
    float atten;
    float3 dif = lighting(pos, normal, eyeDir, specInt, sh, atten);


    float ambInt = 0.5 + 0.5*normal.y;
    float3 amb = ambInt * ambColor;

    float3 color = 1.0 * dif;
    color += 0.2 * amb;

    color *= baseColor;

    float k = 0.02;
    for (float i = 0.0; i < 5.0; ++i)
    {
        float colTemp = i * k - map(pos + normal * i * k).x;
        color -= float3(colTemp, colTemp, colTemp) / pow(2.0, i);
    }

    //specular
    color += pow(specInt, 8.0) * lig1.color * atten * sh;//float3(sh, (sh+sh*sh)*0.5, sh*sh );

    return color;
}

struct PixelShaderOutput
{
    /*float4 position : SV_TARGET0;
    float4 normal : SV_TARGET1;
    float4 color : SV_TARGET2;*/
    float4 color : SV_TARGET;
};

AppendStructuredBuffer<Particle> particleBuffer : register(u2); //start at u2 because u0 & u1 are render target and depth see OMSetRenderTargetsAndUnorderedAccessViews
RWBuffer<uint> indirectDispatchArgs : register(u3);

PixelShaderOutput main(PixelShaderInput input)
{

    PixelShaderOutput output;
    //output.position = float4(-1.0, -1.0, -1.0, -1.0);
    //output.normal = float4(-1.0, -1.0, -1.0, -1.0);
    output.color = float4(0.0, 0.0, 0.0, -1.0);

    Ray r = generateRay(input);

    initScene();

    float3 color = float3(1.3, 1.3, 1.3);
    //float3 color = float3(0.0,0.0,0.0);

    //float4 obj = castRay(r);	
    float4 obj = rm(r.origin, r.direction, 1.0, 0.001, 200.0);

    float3 normal = float3(0.1, 0.1, 0.1); //juste pour quand je teste pour ne pas avoir de trash dans le ciel

    float glowCoeff = 0.01;
    float3 glowColor = float3(1.0, 1.0, 1.0);// * 0.5;
    float3 pos;
    if (obj.w > 0.1)
    {
        pos = r.origin + obj.y * r.direction;
        normal = calcNormal(pos);

        //output.position = float4(pos, 1.0);
        //output.normal = float4(normal, obj.w);


        if (obj.w == 1.0)
        {
            color = ColorMaterial1(pos, normal, obj.x, r.direction);
            applyScattering(color, obj.y);

            float3 ref = reflect(r.direction, normal);
            float4 refObj = rm(pos + 0.002*ref, ref, 1.0, 0.001, 200.0);
            float3 refPos = (pos + 0.002*ref) + refObj.y*ref;

            float3 refNormal = calcNormal(refPos);
            if (refObj.w == 1.0)
            {
                //color = float3(1,0,0);
                color = lerp(color, ColorMaterial1(refPos, refNormal, refObj.x, ref), 0.2);
                applyScattering(color, refObj.y);
            }
            if (refObj.w == 2.0)
            {
                color = lerp(color, ColorMaterial2(refPos, refNormal, refObj.x, ref), 0.2);
                applyScattering(color, refObj.y);
                color = lerp(color, glowColor, refObj.x * glowCoeff);
            }
        }
        if (obj.w == 2.0)
        {
            color = ColorMaterial2(pos, normal, obj.x, r.direction);
            applyScattering(color, obj.y);

            color = lerp(color, glowColor, obj.x * glowCoeff);
        }
        if (obj.w == 3.0)
        {
            color = ColorMaterial3(pos, normal, obj.x, r.direction);
            applyScattering(color, obj.y);

            color = lerp(color, glowColor, obj.x * glowCoeff);

            float3 ref = reflect(r.direction, normal);
            float4 refObj = rm(pos + 0.002*ref, ref, 1.0, 0.001, 200.0);
            float3 refPos = (pos + 0.002*ref) + refObj.y*ref;
            float3 refNormal = calcNormal(refPos);
            if (refObj.w == 1.0)
            {
                //color = float3(1,0,0);
                float3 refColor = ColorMaterial1(refPos, refNormal, refObj.x, ref);
                refColor = lerp(refColor, glowColor, refObj.x * glowCoeff);
                applyScattering(refColor, refObj.y);
                color = lerp(color, refColor, 0.2);

            }
            if (refObj.w == 2.0)
            {
                color = lerp(color, ColorMaterial2(refPos, refNormal, refObj.x, ref), 0.2);
                applyScattering(color, refObj.y);
                color = lerp(color, glowColor, refObj.x * glowCoeff);
            }
        }

        output.color = float4(color, 1.0);
        
        //if (obj.w == 1.0)
        {
            Particle p = (Particle)0;
            p.position = float4(pos, 1.0);
            p.color = float4(color, 1.0);
            p.normal = float4(normal, 1.0);
            particleBuffer.Append(p);

            InterlockedAdd(indirectDispatchArgs[0], 1);
        
            discard;
        }
    }

    color = lerp(color, glowColor, obj.x);

    output.color.xyz = color;

    return output;
}