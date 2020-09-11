#include "../hlsli/ShaderLighting.hlsli"
#include "../hlsli/LightVectorData.hlsli"
#include "../hlsli/PointLight.hlsli"
#include "../hlsli/ShadowPixel.hlsli"

cbuffer ObjectCBuf : register(b1)
{
    float3 specularColor;
    float specularWeight;
    float specularGloss;
    bool useNormalMap;
    float normalMapWeight;
};

Texture2D tex : register(t0);
Texture2D norm : register(t2);
SamplerState smplr : register(s0);

float4 main(float3 viewFragPos : Position, float3 viewNormal : Normal, float3 viewTan : Tangent, float3 viewBitan : Bitangent, float2 tc : Texcoord, float3 sPos : ShadowPosition) : SV_Target
{
    float3 diffuse;
    float3 specular;
    
    if (ShadowUnoccluded(sPos))
    {
        // normalize the meshes normals
        viewNormal = normalize(viewNormal);
    
        // sample normals from normal map
        if ( useNormalMap )
        {
            const float3 mappedNormal = MapNormals(normalize(viewTan), normalize(viewBitan), viewNormal, tc, norm, smplr);
            viewNormal = lerp(viewNormal, mappedNormal, normalMapWeight);
        }
    
	    // fragment to light
        LightVectorData lvd = CalculateLightVectorData(viewLightPos, viewFragPos);
	
	    // attenuation
        const float att = Attenuate(attConst, attLin, attQuad, lvd.distToL);
	
	    // diffuse
        diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lvd.dirToL, viewNormal);
	
	    // specular
        specular = Speculate(diffuseColor * diffuseIntensity * specularColor, specularWeight, viewNormal, lvd.vToL, viewFragPos, att, specularGloss);
    }
    else
    {
        diffuse = specular = 0.0f;
    }
	
	// final color
    return float4(saturate((ambient + diffuse) * tex.Sample(smplr, tc).rgb + specular), 1.0f);
}