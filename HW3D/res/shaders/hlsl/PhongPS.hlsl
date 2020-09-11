#include "../hlsli/ShaderLighting.hlsli"
#include "../hlsli/LightVectorData.hlsli"
#include "../hlsli/PointLight.hlsli"
#include "../hlsli/ShadowPixel.hlsli"

cbuffer ObjectCBuf : register(b1)
{
    float3 materialColor;
    float3 specularColor;
    float specularWeight;
    float specularGloss;
};

float4 main(float3 viewFragPos : Position, float3 viewNormal : Normal, float4 sPos : ShadowPosition) : SV_Target
{
    float3 diffuse;
    float3 specular;
    
    // shadow mapping
    const float shadowLevel = Shadow(sPos);
    if (shadowLevel != 0.0f)
    {
        // renormalize interpolated normal
        viewNormal = normalize(viewNormal);
    
        // fragment to light
        LightVectorData lvd = CalculateLightVectorData(viewLightPos, viewFragPos);
	
	    // attenuation
        const float att = Attenuate(attConst, attLin, attQuad, lvd.distToL);
	
	    // diffuse
        diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lvd.dirToL, viewNormal);
	
	    // specular
        specular = Speculate(diffuseColor * diffuseIntensity * specularColor, specularWeight, viewNormal, lvd.vToL, viewFragPos, att, specularGloss);

        // scale by shadow level
        diffuse *= shadowLevel;
        specular *= shadowLevel;
    }
    else
    {
        diffuse = specular = 0.0f;
    }
	
	// final color
    return float4(saturate((ambient + diffuse) * materialColor + specular), 1.0f);
}