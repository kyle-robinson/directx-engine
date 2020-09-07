#include "../hlsli/ShaderLighting.hlsli"
#include "../hlsli/LightVectorData.hlsli"
#include "../hlsli/PointLight.hlsli"

cbuffer ObjectCBuf
{
    bool useGlossAlpha;
    bool useSpecularMap;
    float3 specularColor;
    float specularWeight;
    float specularGloss;
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
    float specularPowerLoaded = specularGloss;
    const float4 specularSample = spec.Sample(smplr, tc);
    float3 specularReflectionColor = specularColor;
    if ( useSpecularMap )
    {
        specularReflectionColor = specularSample.rgb;
    }
    if (useGlossAlpha)
    {
        specularPowerLoaded = pow(2.0f, specularSample.a * 13.0f);
    }
    
    // specular reflected
    const float3 specularReflected = Speculate(diffuseColor * specularReflectionColor, specularWeight, viewNormal, lvd.vToL, viewFragPos, att, specularPowerLoaded);
	
	// final color
    return float4(saturate((ambient + diffuse) * tex.Sample(smplr, tc).rgb + specularReflected), 1.0f);
}