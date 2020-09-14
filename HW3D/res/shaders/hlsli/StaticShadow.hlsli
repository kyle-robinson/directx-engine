TextureCube sMap : register(t3);
SamplerComparisonState cmpSmplr : register(s1);
SamplerState dmSmplr : register(s2);

float Shadow( const in float4 shadowPos )
{
    float len = length(shadowPos.xyz);
    float4 check = sMap.Sample(dmSmplr, normalize(shadowPos.xyz));
    if (check.r < 1000.0f)
    {
        return sMap.SampleCmpLevelZero(cmpSmplr, normalize(shadowPos.xyz), len / 100.0f);
    }
    else
    {
        return 4.0f;
    }
}