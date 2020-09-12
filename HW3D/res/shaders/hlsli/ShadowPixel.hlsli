Texture2D sMap : register(t3);
SamplerComparisonState sSmplr : register(s1);

cbuffer ShadowControl : register(b2)
{
    int pcfLevel;
    float depthBias;
}

float ShadowLoop( const in float3 sPos, uniform int range )
{
    float shadowLevel = 0.0f;
    [unroll]
    for (int x = -range; x <= range; x++)
    {
        [unroll]
        for (int y = -range; y <= range; y++)
        {
            shadowLevel += sMap.SampleCmpLevelZero(sSmplr, sPos.xy, sPos.b - depthBias, int2(x, y));
        }
    }
    return shadowLevel / ((range * 2 + 1) * (range * 2 + 1));
}

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
        for (int level = 0; level <= 4; level++)
        {
            if (level == pcfLevel)
            {
                shadowLevel = ShadowLoop(sPos, level);
            }
        }
    }
    return shadowLevel;
}