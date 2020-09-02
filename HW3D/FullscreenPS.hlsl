Texture2D tex;
SamplerState smplr;

float4 main( float2 uv : TexCoord ) : SV_Target
{
    return 1.0f - tex.Sample( smplr, uv ).rgba;

}