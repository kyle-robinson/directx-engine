#include "ShaderLighting.hlsli"
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

float4 main(float3 cameraPos : Position, float3 viewNormal : Normal, float3 viewTan : Tangent, float3 viewBitan : Bitangent, float2 tc : Texcoord) : SV_Target
{
    // sample normal from map
    if ( normalMapEnabled )
    {
        viewNormal = MapNormals(viewTan, viewBitan, viewNormal, tc, norm, smplr);
    }
    
	// fragment to light
    const float3 viewFragToL = viewLightPos - cameraPos;
    const float distFragToL = length(viewFragToL);
    const float3 viewDirFragToL = viewFragToL / distFragToL;
	
	// diffuse attenuation
    const float att = Attenuate(attConst, attLin, attQuad, distFragToL);
	
	// diffuse intensity
    const float3 diffuse = Diffuse(diffuseColor, diffuseIntensity, att, viewDirFragToL, viewNormal);
	
	// specular intensity
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
    const float3 specularReflected = Speculate(specularReflectionColor, 1.0f, viewNormal, viewFragToL, cameraPos, att, specularPower);
	
	// final color
    return float4(saturate((ambient + diffuse) * tex.Sample(smplr, tc).rgb + specularReflected), 1.0f);
}