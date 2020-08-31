#include "ShaderLighting.hlsli"
#include "LightVectorData.hlsli"
#include "PointLight.hlsli"

cbuffer ObjectCBuf
{
    float4 materialColor;
    float4 specularColor;
    float specularPower;
};

float4 main(float3 viewFragPos : Position, float3 viewNormal : Normal) : SV_Target
{
	// renormalize interpolated normal
    viewNormal = normalize(viewNormal);
    
    // fragment to light
    LightVectorData lvd = CalculateLightVectorData(viewLightPos, viewFragPos);
	
	// attenuation
    const float att = Attenuate(attConst, attLin, attQuad, lvd.distToL);
	
	// diffuse
    const float3 diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lvd.dirToL, viewNormal);
	
	// specular
    const float3 specular = Speculate(specularColor.rgb, 1.0f, viewNormal, lvd.vToL, viewFragPos, att, specularPower);
	
	// final color
    return float4(saturate((ambient + diffuse) * materialColor.rgb + specular), 1.0f);
}