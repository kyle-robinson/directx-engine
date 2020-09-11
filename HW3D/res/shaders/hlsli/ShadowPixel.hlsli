Texture2D sMap : register(t3);
SamplerState sSmplr : register(s1);

bool ShadowUnoccluded( const in float3 shadowScreenPos )
{
    return sMap.Sample(sSmplr, shadowScreenPos.xy).r > shadowScreenPos.z - 0.005f;
}