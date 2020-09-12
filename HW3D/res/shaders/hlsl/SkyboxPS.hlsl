TextureCube texCube : register(t0);
SamplerState smplr : register(s0);

float4 main( float3 worldPos : Position ) : SV_Target
{
    return texCube.Sample(smplr, worldPos);
}