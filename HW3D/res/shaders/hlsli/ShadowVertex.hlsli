cbuffer ShadowTransformCbuf : register(b1)
{
    matrix shadowPos;
}

float4 ToShadowScreenSpace( const in float3 pos, uniform matrix modelTransform )
{
    const float4 worldPos = mul(float4(pos, 1.0f), modelTransform);
    return mul( worldPos, shadowPos );
}