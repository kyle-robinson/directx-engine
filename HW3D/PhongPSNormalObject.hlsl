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
    float speuclarIntensity;
    float specularPower;
    bool normalMapEnabled;
    float padding[1];
};

cbuffer TransformCBuf
{
    matrix modelView;
    matrix modelViewProj;
};

Texture2D tex;
Texture2D norm : register(t2);

SamplerState smplr;

float4 main(float3 cameraPos : Position, float3 viewNormal : Normal, float2 tc : Texcoord) : SV_Target
{
    // sample normals from normal map
    if ( normalMapEnabled )
    {
        // get normals from map into tangent space
        const float3 normalSample = norm.Sample( smplr, tc ).xyz;
        float3 tanNormal;
        tanNormal.x = normalSample.x * 2.0f - 1.0f;
        tanNormal.y = -normalSample.y * 2.0f + 1.0f;
        tanNormal.z = -normalSample.z * 2.0f + 1.0f;
        viewNormal = mul(tanNormal, (float3x3) modelView);
    }
    
	// fragment to light
    const float3 vToL = lightPos - cameraPos;
    const float distToL = length(vToL);
    const float3 dirToL = vToL / distToL;
	
	// diffuse attenuation
    const float att = 1.0f / (attConst + attLin * distToL + attQuad * (distToL * distToL));
	
	// diffuse intensity
    const float3 diffuse = diffuseColor * diffuseIntensity * att * max(0.0f, dot(dirToL, viewNormal));
	
	// reflected light vector
    const float3 w = viewNormal * dot(vToL, viewNormal);
    const float3 r = w * 2.0f - vToL;
	
	// specular intensity
    const float3 specular = ( diffuseColor * diffuseIntensity ) * att * pow(max(0.0f, dot(normalize(-r), normalize(cameraPos))), specularPower);
	
	// final color
    return float4(saturate((ambient + diffuse) * tex.Sample(smplr, tc).rgb + specular), 1.0f);
}