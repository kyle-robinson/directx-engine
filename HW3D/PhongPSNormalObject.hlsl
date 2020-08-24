#include "ShaderLighting.hlsli"
#include "LightVectorData.hlsli"
#include "PointLight.hlsli"

cbuffer ObjectCBuf
{
    float specularIntensity;
    float specularPower;
    bool normalMapEnabled;
    float padding[1];
};

#include "Transform.hlsli"

Texture2D tex;
Texture2D norm : register(t2);

SamplerState smplr;

float4 main(float3 viewFragPos : Position, float3 viewNormal : Normal, float2 tc : Texcoord) : SV_Target
{
    // sample normals from normal map
    if ( normalMapEnabled )
    {
        // get normals from map into tangent space
        const float3 normalSample = norm.Sample( smplr, tc ).xyz;
        const float3 objectNormal = normalSample * 2.0f - 1.0f;
        
        // normal from object to view space
        viewNormal = normalize(mul(objectNormal, (float3x3) modelView));
    }
    
	// fragment to light
    const LightVectorData lvd = CalculateLightVectorData( viewLightPos, viewFragPos );
	
	// diffuse attenuation
    const float att = Attenuate(attConst, attLin, attQuad, lvd.distToL);
	
	// diffuse intensity
    const float3 diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lvd.dirToL, viewNormal);
	
	// specular intensity
    const float3 specular = Speculate(specularIntensity.rrr, 1.0f, viewNormal, lvd.vToL, viewFragPos, att, specularPower);
	
	// final color
    return float4(saturate((ambient + diffuse) * tex.Sample(smplr, tc).rgb + specular), 1.0f);
}