#include "ShaderLighting.hlsli"
#include "LightVectorData.hlsli"
#include "PointLight.hlsli"

cbuffer ObjectCBuf
{
    float speuclarIntensity;
    float specularPower;
    bool normalMapEnabled;
    float padding[1];
};

Texture2D tex;
Texture2D norm : register(t2);
SamplerState smplr;

float4 main(float3 viewFragPos : Position, float3 viewNormal : Normal, float3 viewTan : Tangent, float3 viewBitan : Bitangent, float2 tc : Texcoord) : SV_Target
{
    // normalize the meshes normals
    viewNormal = normalize(viewNormal);
    
    // sample normals from normal map
    if ( normalMapEnabled )
    {
        viewNormal = MapNormals(normalize(viewTan), normalize(viewBitan), viewNormal, tc, norm, smplr);
    }
    
	// fragment to light
    LightVectorData lvd = CalculateLightVectorData(viewLightPos, viewFragPos);
	
	// attenuation
    const float att = Attenuate(attConst, attLin, attQuad, lvd.distToL);
	
	// diffuse
    const float3 diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lvd.dirToL, viewNormal);
	
	// specular
    const float3 specular = Speculate(diffuseColor, diffuseIntensity, viewNormal, lvd.vToL, viewFragPos, att, specularPower);
	
	// final color
    return float4(saturate((ambient + diffuse) * tex.Sample(smplr, tc).rgb + specular), 1.0f);
}