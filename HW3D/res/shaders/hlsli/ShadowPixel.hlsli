Texture2D sMap : register(t3);
SamplerComparisonState sSmplr : register(s1);

#define PCF_RANGE 2

float Shadow( const in float4 shadowScreenPos )
{
    float shadowLevel = 0.0f;
    const float3 sPos = shadowScreenPos.xyz / shadowScreenPos.w;
    if ( sPos.z > 1.0f || sPos.z < 0.0f )
    {
        shadowLevel = 1.0f;
    }
    else
    {
       [unroll]
        for (int x = -PCF_RANGE; x <= PCF_RANGE; x++)
        {
            [unroll]
            for (int y = -PCF_RANGE; y <= PCF_RANGE; y++)
            {
                shadowLevel += sMap.SampleCmpLevelZero(sSmplr, sPos.xy, sPos.b - 0.0005f, int2(x, y));
            }
        }
        shadowLevel /= ((PCF_RANGE * 2 + 1) * (PCF_RANGE * 2 + 1));
    }
    return shadowLevel;
}