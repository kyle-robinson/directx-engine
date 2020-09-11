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
    bool useNormalMap;
    float normalMapWeight;
};

Texture2D tex : register(t0);
Texture2D spec : register(t1);
Texture2D norm : register(t2);

SamplerState smplr : register(s0);

float4 main(float3 viewFragPos : Position, float3 viewNormal : Normal, float3 viewTan : Tangent, float3 viewBitan : Bitangent, float2 tc : Texcoord, float4 sPos : ShadowPosition) : SV_Target
{
    // sample diffuse texture
    const float4 dtex = tex.Sample(smplr, tc);
    
#ifdef MASK_ENABLED
    clip(dtex.a < 0.1f ? -1 : 1);
    
    // flip normals for backfaces
    if (dot(viewNormal, viewFragPos) >= 0.0f)
    {
        viewNormal = -viewNormal;
    }
#endif
    
    float3 diffuse;
    float3 specularReflected;
    
    if (ShadowUnoccluded(sPos))
    {
        // sample normal from map
        viewNormal = normalize(viewNormal);
        if ( useNormalMap )
        {
            const float3 mappedNormal = MapNormals(normalize(viewTan), normalize(viewBitan), viewNormal, tc, norm, smplr);
            viewNormal = lerp( viewNormal, mappedNormal, normalMapWeight );
        }
    
	    // fragment to light
        const LightVectorData lvd = CalculateLightVectorData(viewLightPos, viewFragPos);
	
	    // diffuse attenuation
        const float att = Attenuate(attConst, attLin, attQuad, lvd.distToL);
	
	    // diffuse intensity
        diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lvd.dirToL, viewNormal);
	
	    // specular - uniform or mapped
        float3 specularReflectionColor;
        float specularPower = specularGloss;
        const float4 specularSample = spec.Sample(smplr, tc);
        if ( useSpecularMap )
        {
            specularReflectionColor = specularSample.rgb;
        }
        else
        {
            specularReflectionColor = specularColor;
        }
        if ( useGlossAlpha )
        {
            specularPower = pow(2.0f, specularSample.a * 13.0f);
        }
    
        // specular reflected
        specularReflected = Speculate(diffuseColor * diffuseIntensity * specularReflectionColor, specularWeight, viewNormal, lvd.vToL, viewFragPos, att, specularPower);
    }
    else
    {
        diffuse = specularReflected = 0.0f;
    }
    
    
	// final color
    return float4(saturate((ambient + diffuse) * dtex.rgb + specularReflected), 1.0f);
}