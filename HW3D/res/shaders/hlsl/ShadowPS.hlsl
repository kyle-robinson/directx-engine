#include "../hlsli/ShaderLighting.hlsli"
#include "../hlsli/LightVectorData.hlsli"
#include "../hlsli/PointLight.hlsli"
#include "../hlsli/ShadowPixel.hlsli"

cbuffer ObjectCbuf : register(b1)
{
    float3 specularColor;
    float specularWeight;
    float specularGloss;
};

Texture2D tex : register(t0);
SamplerState smplr : register(s0);

float4 main( float3 viewFragPos : Position, float3 viewNormal : Normal, float2 tc : Texcoord, float3 sPos : ShadowPosition ) : SV_Target
{
    float3 diffuse;
    float3 specular;
    
    // shadow mapping
    if (ShadowUnoccluded(sPos))
    {
        // renormalize interpolated normal
        viewNormal = normalize(viewNormal);
        
        // frag to light vector data
        const LightVectorData lvd = CalculateLightVectorData(viewLightPos, viewFragPos);
        
        // attenuation
        const float att = Attenuate(attConst, attLin, attQuad, lvd.distToL);
        
        // diffuse
        diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lvd.dirToL, viewNormal);
        
        // specular
        specular = Speculate(diffuseColor * diffuseIntensity * specularColor, specularWeight, viewNormal, lvd.vToL, viewFragPos, att, specularGloss);

    }
    else
    {
        diffuse = specular = float3(0.0f, 0.0f, 0.0f);
    }
    
    return float4(saturate((diffuse + ambient) * tex.Sample(smplr, tc).rgb + specular), 1.0f);
}