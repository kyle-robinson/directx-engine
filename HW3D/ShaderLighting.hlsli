float3 MapNormals(
    const float3 tan,
    const float3 bitan,
    const float3 normal,
    const float2 tc,
    Texture2D nmap,
    SamplerState smplr)
{
    // build the rotation matrix into tangent space
    const float3x3 tanToTarget = float3x3(tan, bitan, normal);
        
        // get normal data from map
    const float3 normalSample = nmap.Sample(smplr, tc).xyz;
    float3 tanNormal;
    tanNormal = normalSample * 2.0f - 1.0f;
        
        // normal from tangent to view
    return normalize(mul(tanNormal, tanToTarget));
}

float Attenuate(
    uniform float attConst,
    uniform float attLin,
    uniform float attQuad,
    const in float distFragToL)
{
    return 1.0f / (attConst + attLin * distFragToL + attQuad * (distFragToL * distFragToL));
}

float3 Diffuse(
    uniform float3 diffuseColor,
    uniform float diffuseIntensity,
    const in float att,
    const in float3 viewDirFragToL,
    const in float3 viewNormal)
{
    return diffuseColor * diffuseIntensity * att * max(0.0f, dot(viewDirFragToL, viewNormal));
}

float3 Speculate(
    uniform float3 specularColor,
    uniform float specularIntensity,
    const in float3 viewNormal,
    const in float3 viewFragToL,
    const in float3 cameraPos,
    const in float att,
    const in float specularPower)
{
    // reflected light vector
    const float3 w = viewNormal * dot(viewFragToL, viewNormal);
    const float3 r = w * 2.0f - viewFragToL;
    
    return specularColor * specularIntensity * att * pow(max(0.0f, dot(normalize(-r), normalize(cameraPos))), specularPower);
}