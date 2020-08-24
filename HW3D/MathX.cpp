#include "MathX.h"

DirectX::XMFLOAT3 ExtractEulerAngles( const DirectX::XMFLOAT4X4& matrix )
{
	DirectX::XMFLOAT3 euler;

	euler.x = asinf( -matrix._32 );
	if ( cosf( euler.x ) > 0.0001 )
	{
		euler.y = atan2f( matrix._31, matrix._33 );
		euler.z = atan2f( matrix._12, matrix._22 );
	}
	else
	{
		euler.y = 0.0f;
		euler.z = atan2f( -matrix._21, matrix._11 );
	}

	return euler;
}

DirectX::XMFLOAT3 ExtractTranslation( const DirectX::XMFLOAT4X4& matrix )
{
	return { matrix._41, matrix._42, matrix._43 };
}