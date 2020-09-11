Texture2D sMap : register(t3);
SamplerState sSmplr : register(s1);

float Shadow( const in float4 shadowScreenPos )
{
    float shadowLevel = 0.0f;
    const float3 sPos = shadowScreenPos.xyz / shadowScreenPos.w;
    if ( sPos.z > 1.0f )
    {
        shadowLevel = 1.0f;
    }
    else
    {
        // dimensions for pixel sampling
        uint width, height;
        sMap.GetDimensions( width, height );
        const float dx = 0.5f / width;
        const float dy = 0.5f / height;
        
        // sample 4 pixles around shadow map position
        const float depthBias = sPos.z - 0.0005f;
        shadowLevel += sMap.Sample(sSmplr, sPos.xy + float2( dx,  dy)).r >= depthBias ? 0.25f : 0.0f;
        shadowLevel += sMap.Sample(sSmplr, sPos.xy + float2(-dx,  dy)).r >= depthBias ? 0.25f : 0.0f;
        shadowLevel += sMap.Sample(sSmplr, sPos.xy + float2( dx, -dy)).r >= depthBias ? 0.25f : 0.0f;
        shadowLevel += sMap.Sample(sSmplr, sPos.xy + float2(-dx, -dy)).r >= depthBias ? 0.25f : 0.0f;
    }
    return shadowLevel;
}