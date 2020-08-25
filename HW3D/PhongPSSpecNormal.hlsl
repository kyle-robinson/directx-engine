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
Texture2D norm;

SamplerState smplr;

float4 main(float3 viewFragPos : Position, float3 viewNormal : Normal, float3 viewTan : Tangent, float3 viewBitan : Bitangent, float2 tc : Texcoord) : SV_Target
{
    // sample diffuse texture
    float4 dtex = tex.Sample(smplr, tc);
    clip(dtex.a < 0.1f ? -1 : 1);
    
    // flip normals for backfaces
    if (dot(viewNormal, viewFragPos) >= 0.0f)
    {
        viewNormal = -viewNormal;
    }
    
    // sample normal from map
    viewNormal = normalize(viewNormal);
    if ( normalMapEnabled )
    {
        viewNormal = MapNormals(normalize(viewTan), normalize(viewBitan), viewNormal, tc, norm, smplr);
    }
    
	// fragment to light
    const LightVectorData lvd = CalculateLightVectorData(viewLightPos, viewFragPos);
	
	// diffuse attenuation
    const float att = Attenuate(attConst, attLin, attQuad, lvd.distToL);
	
	// diffuse intensity
    const float3 diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lvd.dirToL, viewNormal);
	
	// specular - uniform or mapped
    float3 specularReflectionColor;
    float specularPower = specularPowerConst;
    if ( specularMapEnabled )
    {
        const float4 specularSample = spec.Sample(smplr, tc);
        specularReflectionColor = specularSample.rgb * specularMapWeight;
        if ( hasGloss )
        {
            specularPower = pow(2.0f, specularSample.a * 13.0f);
        }
    }
    else
    {
        specularReflectionColor = specularColor;
    }
    
    // specular reflected
    const float3 specularReflected = Speculate(specularReflectionColor, 1.0f, viewNormal, lvd.vToL, viewFragPos, att, specularPower);
    
	// final color
    return float4(saturate((ambient + diffuse) * dtex.rgb + specularReflected), dtex.a);
}