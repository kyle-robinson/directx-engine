TextureCube sMap : register(t3);
SamplerComparisonState cmpSmplr : register(s1);

float Shadow( const in float4 shadowPos )
{
    return sMap.SampleCmpLevelZero(cmpSmplr, normalize(shadowPos.xyz), length(shadowPos.xyz) / 100.0f);
}