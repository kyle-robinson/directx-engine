cbuffer LightCBuf
{
    float3 lightPos;
    float3 ambient;
    float3 diffuseColor;
    float diffuseIntensity;
    float attConst;
    float attLin;
    float attQuad;
};

cbuffer ObjectCBuf
{
    bool normalMapEnabled;
    bool specularMapEnabled;
    bool hasGloss;
    float specularPowerConst;
    float3 specularColor;
    float specularMapWeight;
};

Texture2D tex;
Texture2D spec;
Texture2D norm;

SamplerState smplr;

float4 main(float3 cameraPos : Position, float3 n : Normal, float3 tan : Tangent, float3 bitan : Bitangent, float2 tc : Texcoord) : SV_Target
{
    // sample normal from map
    if ( normalMapEnabled )
    {
        // build the rotation matrix into tangent space
        const float3x3 tanToView = float3x3(
            normalize(tan),
            normalize(bitan),
            normalize(n)
        );
        
        // get normal data from map
        const float3 normalSample = norm.Sample(smplr, tc).xyz;
        n = normalSample * 2.0f - 1.0f;
        
        // normal from tangent to view
        n = mul(n, tanToView);
    }
    
	// fragment to light
    const float3 vToL = lightPos - cameraPos;
    const float distToL = length(vToL);
    const float3 dirToL = vToL / distToL;
	
	// diffuse attenuation
    const float att = 1.0f / (attConst + attLin * distToL + attQuad * (distToL * distToL));
	
	// diffuse intensity
    const float3 diffuse = diffuseColor * diffuseIntensity * att * max(0.0f, dot(dirToL, n));
	
	// reflected light vector
    const float3 w = n * dot(vToL, n);
    const float3 r = w * 2.0f - vToL;
	
	// specular intensity
    float3 specularReflectionColor;
    float specularPower = specularPowerConst;
    if ( specularMapEnabled )
    {
        const float4 specularSample = spec.Sample(smplr, tc);
        specularReflectionColor = specularSample.rgb * specularMapWeight;
        if ( hasGloss )
        {
            specularPower = pow(2.0f, specularSample.a * 13.0f);
        }
    }
    else
    {
        specularReflectionColor = specularColor;
    }
    const float3 specular = (diffuseColor * diffuseIntensity) * att * pow(max(0.0f, dot(normalize(-r), normalize(cameraPos))), specularPower);
	
	// final color
    return float4(saturate((ambient + diffuse) * tex.Sample(smplr, tc).rgb + specular * specularReflectionColor), 1.0f);
}