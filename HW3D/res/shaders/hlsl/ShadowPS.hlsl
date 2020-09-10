#include "../hlsli/ShaderLighting.hlsli"
#include "../hlsli/LightVectorData.hlsli"
#include "../hlsli/PointLight.hlsli"

cbuffer ObjectCbuf
{
    float3 specularColor;
    float specularWeight;
    float specularGloss;
};

Texture2D tex;
Texture2D sMap : register(t3);

SamplerState smplr;
SamplerState sSmplr;

float4 main( float3 viewFragPos : Position, float3 viewNormal : Normal, float2 tc : Texcoord, float4 sPos : ShadowPosition ) : SV_Target
{
    float3 diffuse;
    float3 specular;
    
    // shadow mapping
    sPos.xyz = sPos.xyz / sPos.w;
    if (sMap.Sample(sSmplr, sPos.xy).r > sPos.z)
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