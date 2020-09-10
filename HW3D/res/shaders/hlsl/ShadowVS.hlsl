#include "../hlsli/Transform.hlsli"

cbuffer ShadowTransform
{
    matrix shadowView;
};

struct VSOut
{
    float3 viewPos : Position;
    float3 viewNormal : Normal;
    float2 tc : Texcoord;
    float4 worldPos : ShadowPosition;
    float4 pos : SV_Position;
};

VSOut main( float3 pos : Position, float3 n : Normal, float2 tc : Texcoord )
{
    VSOut vso;
    vso.viewPos = (float3) mul(float4(pos, 1.0f), modelView);
    vso.viewNormal = mul(n, (float3x3)modelView);
    vso.tc = tc;
    
    const float4 shadowCamera = mul(float4(pos, 1.0f), model);
    const float4 shadowNDC = mul(shadowCamera, shadowView);
    vso.worldPos = shadowNDC * float4(0.5f, -0.5f, 1.0f, 1.0f) + (float4(0.5f, 0.5f, 0.0f, 0.0f) * shadowNDC.w);
    
    vso.pos = mul(float4(pos, 1.0f), modelViewProj);
    
    return vso;
}