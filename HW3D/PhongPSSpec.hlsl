#include "ShaderLighting.hlsli"
#include "LightVectorData.hlsli"
#include "PointLight.hlsli"

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

SamplerState smplr;

float4 main(float3 viewFragPos : Position, float3 viewNormal : Normal, float2 tc : Texcoord) : SV_Target
{
    // sample normal from map
    viewNormal = normalize(viewNormal);
    
	// fragment to light
    const LightVectorData lvd = CalculateLightVectorData(viewLightPos, viewFragPos);
	
	// diffuse attenuation
    const float att = Attenuate(attConst, attLin, attQuad, lvd.distToL);
	
	// diffuse intensity
    const float3 diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lvd.dirToL, viewNormal);
	
	// specular - uniform or mapped
    float specularPower = specularPowerConst;
    const float4 specularSample = spec.Sample(smplr, tc);
    const float3 specularReflectionColor = specularSample.rgb * specularMapWeight;
    if ( hasGloss )
    {
        specularPower = pow(2.0f, specularSample.a * 13.0f);
    }
    
    // specular reflected
    const float3 specularReflected = Speculate(specularReflectionColor, 1.0f, viewNormal, lvd.vToL, viewFragPos, att, specularPower);
	
	// final color
    return float4(saturate((ambient + diffuse) * tex.Sample(smplr, tc).rgb + specularReflected), 1.0f);
}