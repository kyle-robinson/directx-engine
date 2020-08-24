struct LightVectorData
{
    float3 vToL;
    float distToL;
    float3 dirToL;
};

LightVectorData CalculateLightVectorData(const in float3 lightPos, const in float3 fragPos)
{
    LightVectorData lvd;
    lvd.vToL = lightPos - fragPos;
    lvd.distToL = length( lvd.vToL );
    lvd.dirToL = lvd.vToL / lvd.distToL;
    return lvd;
}