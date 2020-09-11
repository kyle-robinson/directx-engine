#include "../hlsli/ShaderLighting.hlsli"
#include "../hlsli/LightVectorData.hlsli"
#include "../hlsli/PointLight.hlsli"
#include "../hlsli/ShadowPixel.hlsli"

cbuffer ObjectCBuf : register(b1)
{
    bool useGlossAlpha;
    bool useSpecularMap;
    float3 specularColor;
    float specularWeight;
    float specularGloss;
};

Texture2D tex : register(t0);
Texture2D spec : register(t1);

SamplerState smplr : register(s0);

float4 main(float3 viewFragPos : Position, float3 viewNormal : Normal, float2 tc : Texcoord, float4 sPos : ShadowPosition) : SV_Target
{
    float3 diffuse;
    float3 specularReflected;
    
    // shadow mapping
    const float shadowLevel = Shadow(sPos);
    if (shadowLevel != 0.0f)
    {
        // sample normal from map
        viewNormal = normalize(viewNormal);
    
	    // fragment to light
        const LightVectorData lvd = CalculateLightVectorData(viewLightPos, viewFragPos);
	
	    // diffuse attenuation
        const float att = Attenuate(attConst, attLin, attQuad, lvd.distToL);
	
	    // diffuse intensity
        diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lvd.dirToL, viewNormal);
	
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
        specularReflected = Speculate(diffuseColor * specularReflectionColor, specularWeight, viewNormal, lvd.vToL, viewFragPos, att, specularPowerLoaded);

        // scale by shadow level
        diffuse *= shadowLevel;
        specularReflected *= shadowLevel;
    }
    else
    {
        diffuse = specularReflected = 0.0f;
    }
    
	
	// final color
    return float4(saturate((ambient + diffuse) * tex.Sample(smplr, tc).rgb + specularReflected), 1.0f);
}