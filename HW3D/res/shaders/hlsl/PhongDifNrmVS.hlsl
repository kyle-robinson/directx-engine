#include "../hlsli/Transform.hlsli"
#include "../hlsli/ShadowVertex.hlsli"

struct VSOut
{
    float3 cameraPos : Position;
    float3 viewNormal : Normal;
    float3 tan : Tangent;
    float3 bitan : Bitangent;
    float2 tc : Texcoord;
    float3 shadowCamScreen : ShadowPosition;
    float4 pos : SV_Position;
};

VSOut main(float3 pos : Position, float3 n : Normal, float2 tc : Texcoord, float3 tan : Tangent, float3 bitan : Bitangent)
{
    VSOut vso;
    vso.cameraPos = (float3) mul(float4(pos, 1.0f), modelView);
    vso.viewNormal = mul(n, (float3x3) modelView);
    vso.tan = mul(tan, (float3x3) modelView);
    vso.bitan = mul(bitan, (float3x3) modelView);
    vso.tc = tc;
    vso.shadowCamScreen = ToShadowScreenSpace(pos, model);
    vso.pos = mul(float4(pos, 1.0f), modelViewProj);
    return vso;
}