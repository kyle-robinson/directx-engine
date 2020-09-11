Texture2D sMap : register(t3);
SamplerState sSmplr : register(s1);

bool ShadowUnoccluded( const in float4 shadowScreenPos )
{
    const float3 sPos = shadowScreenPos.xyz / shadowScreenPos.w;
    return sPos.z > 1.0f ? true : sMap.Sample(sSmplr, sPos.xy).r > sPos.z - 0.00005f;
}