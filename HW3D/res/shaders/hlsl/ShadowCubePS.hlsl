float4 main( float3 viewPos : Position ) : SV_Target
{
    return length(viewPos) / 100.0f + 0.005f;

}