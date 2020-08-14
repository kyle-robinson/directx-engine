cbuffer LightCBuf
{
	float3 lightPos;
	float3 ambient;
	float3 diffuseColor;
	float diffuseIntensity;
	float attConst;
	float attLin;
	float attQuad;
};

cbuffer ObjectCBuf
{
	float3 materialColor;
	float specularIntensity;
	float specularPower;
};

float4 main( float3 cameraPos : Position, float3 n : Normal ) : SV_Target
{
	// fragment to light
	const float3 vToL = lightPos - cameraPos;
	const float distToL = length( vToL );
	const float3 dirToL = vToL / distToL;
	
	// diffuse attenuation
	const float att = 1.0f / ( attConst + attLin * distToL + attQuad * ( distToL * distToL ) );
	
	// diffuse intensity
	const float3 diffuse = diffuseColor * diffuseIntensity * att * max( 0.0f, dot( dirToL, n ) );
	
	// reflected light vector
	const float3 w = n * dot( vToL, n );
	const float3 r = w * 2.0f - vToL;
	
	// specular intensity
	const float3 specular = diffuse * specularIntensity * att * pow( max( 0.0f, dot( normalize( -r ), normalize( cameraPos ) ) ), specularPower );
	
	// final color
	return float4( saturate( ( ambient + diffuse + specular ) * materialColor), 1.0f);
}