#include "../hlsli/ShaderLighting.hlsli"
#include "../hlsli/LightVectorData.hlsli"
#include "../hlsli/PointLight.hlsli"

cbuffer ObjectCBuf
{
    float3 specularColor;
    float specularWeight;
    float specularGloss;
};

Texture2D tex;
SamplerState smplr;

float4 main(float3 viewFragPos : Position, float3 viewNormal : Normal, float2 tc : Texcoord) : SV_Target
{
	// renormalize interpolated normal
    viewNormal = normalize(viewNormal);
    
    // fragment to light
    const LightVectorData lvd = CalculateLightVectorData(viewLightPos, viewFragPos);
	
	// attenuation
    const float att = Attenuate(attConst, attLin, attQuad, lvd.distToL);
	
	// diffuse
    const float3 diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lvd.dirToL, viewNormal);
	
	// specular
    const float3 specular = Speculate(diffuseColor * diffuseIntensity * specularColor, specularWeight, viewNormal, lvd.vToL, viewFragPos, att, specularGloss);
	
	// final color
    return float4(saturate((ambient + diffuse) * tex.Sample(smplr, tc).rgb + specular), 1.0f);
}