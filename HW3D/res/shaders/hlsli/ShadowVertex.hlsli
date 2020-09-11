cbuffer ShadowTransformCbuf : register(b1)
{
    matrix shadowViewProj;
}

float4 ToShadowScreenSpace( const in float3 pos, uniform matrix modelTransform )
{
    const float4 worldPos = mul(float4(pos, 1.0f), modelTransform);
    const float4 shadowNDC = mul(worldPos, shadowViewProj);
    return shadowNDC * float4(0.5f, -0.5f, 1.0f, 1.0f) + float4(0.5f, 0.5f, 0.0f, 0.0f) * shadowNDC.w;
}