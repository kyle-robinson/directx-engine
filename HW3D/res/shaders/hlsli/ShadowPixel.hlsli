//Texture2D sMap : register(t3);
TextureCube sMap : register(t3);
SamplerComparisonState sSmplrHw : register(s1);
//SamplerState sSmplr : register(s2);

/*cbuffer ShadowControl : register(b2)
{
    int pcfLevel;
    float depthBias;
    bool hwPcf;
}*/

//float ShadowLoop( const in float3 sPos, uniform int range )
float ShadowLoop( const in float4 sPos )
{
    /*float shadowLevel = 0.0f;
    [unroll]
    for (int x = -range; x <= range; x++)
    {
        [unroll]
        for (int y = -range; y <= range; y++)
        {
            if (hwPcf)
                shadowLevel += sMap.SampleCmpLevelZero(sSmplrHw, sPos.xy, sPos.b - depthBias, int2(x, y));
            else
                shadowLevel += sMap.Sample(sSmplr, sPos.xy, int2(x, y)).r >= sPos.b - depthBias ? 1.0f : 0.0f;
        }
    }
    return shadowLevel / ((range * 2 + 1) * (range * 2 + 1));*/
    
    return sMap.SampleCmpLevelZero(sSmplrHw, sPos.xyz, length(sPos.xyz) / 100.0f);
}

float Shadow( const in float4 shadowScreenPos )
{
    /*float shadowLevel = 0.0f;
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
    }*/
    //return shadowLevel;
    return ShadowLoop( shadowScreenPos );

}