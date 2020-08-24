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
    float4 materialColor;
    float4 specularColor;
    float specularPower;
};

float4 main(float3 cameraPos : Position, float3 viewNormal : Normal) : SV_Target
{
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
    const float4 specular = (float4(diffuseColor, 1.0f) * diffuseIntensity) * specularColor * att * pow(max(0.0f, dot(normalize(-r), normalize(cameraPos))), specularPower);
	
	// final color
    return saturate(float4(ambient + diffuse, 1.0f) * materialColor + specular);
}