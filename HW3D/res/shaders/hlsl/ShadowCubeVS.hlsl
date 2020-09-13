#include "../hlsli/Transform.hlsli"

float4 main( float3 pos : Position ) : SV_Position
{
    float4 output = mul(float4(pos, 1.0f), modelViewProj);
    const float3 viewPos = mul(float4(pos, 1.0f), modelView).xyz;
    output.z = length(viewPos) * output.w / 100.0f;
    return output;
}