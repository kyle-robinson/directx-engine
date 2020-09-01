#include "TexturePreprocessor.h"
#include "ModelException.h"
#include "Math.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <sstream>
#include <filesystem>

void TexturePreprocessor::FlipAllYNormalsInObj( const std::string& path )
{
	const auto rootPath = std::filesystem::path{ path.c_str() }.parent_path().string() + "\\";

	// load .obj to get list of normal maps from the materials
	Assimp::Importer importer;
	const auto pScene = importer.ReadFile( path.c_str(), 0u );
	if ( pScene == nullptr )
		throw ModelException( __LINE__, __FILE__, importer.GetErrorString() );

	// loop through materials and process normal maps
	for ( auto i = 0u; i < pScene->mNumMaterials; i++ )
	{
		aiString texFileName;
		const auto& mat = *pScene->mMaterials[i];
		if ( mat.GetTexture( aiTextureType_NORMALS, 0, &texFileName ) == aiReturn_SUCCESS )
		{
			const auto path = rootPath + texFileName.C_Str();
			FlipYNormalMap( path, path );
		}
	}
}

void TexturePreprocessor::FlipYNormalMap( const std::string& pathIn, const std::string& pathOut )
{
	// process each normal in texture
	const auto flipY = DirectX::XMVectorSet(1.0f, -1.0f, 1.0f, 1.0f);
	const auto ProcessNormal = [flipY]( DirectX::FXMVECTOR n, int x, int y ) -> DirectX::XMVECTOR
	{
		return DirectX::XMVectorMultiply(n, flipY);
	};
	// execute process for every texel in file
	TransformFile( pathIn, pathOut, ProcessNormal );
}

void TexturePreprocessor::ValidateNormalMap( const std::string& pathIn, float thresholdMin, float thresholdMax )
{
	OutputDebugStringA( ( "Validating Normal Map [" + pathIn + "]\n" ).c_str() );

	// process each normal in texture
	auto sum = DirectX::XMVectorZero();
	const auto ProcessNormal = [thresholdMin, thresholdMax, &sum]( DirectX::FXMVECTOR n, int x, int y ) -> DirectX::XMVECTOR
	{
		const float len = DirectX::XMVectorGetX( DirectX::XMVector3Length( n ) );
		const float z = DirectX::XMVectorGetZ( n );
		if ( len < thresholdMin || len > thresholdMax )
		{
			DirectX::XMFLOAT3 vec;
			DirectX::XMStoreFloat3( &vec, n );
			std::ostringstream oss;
			oss << "Bad Normal Length: " << len << " at: (" << x << "," << y << ") normal: (" << vec.x << "," << vec.y << "," << vec.z << ")\n";
			OutputDebugStringA( oss.str().c_str() );
		}
		if ( z < 0.0f )
		{
			DirectX::XMFLOAT3 vec;
			DirectX::XMStoreFloat3( &vec, n );
			std::ostringstream oss;
			oss << "Bad Normal Z Direction at: (" << x << "," << y << ") normal: (" << vec.x << "," << vec.y << "," << vec.z << ")\n";
			OutputDebugStringA( oss.str().c_str() );
		}
		sum = DirectX::XMVectorAdd( sum, n );
		return n;
	};
}

template<typename F>
inline void TexturePreprocessor::TransformFile( const std::string& pathIn, const std::string& pathOut, F&& func )
{
	auto surface = Surface::FromFile( pathIn );
	TransformSurface( surface, func );
	surface.Save( pathOut );
}

template<typename F>
inline void TexturePreprocessor::TransformSurface( Surface& surface, F&& func )
{
	const auto width = surface.GetWidth();
	const auto height = surface.GetHeight();
	for ( unsigned int y = 0; y < height; y++ )
	{
		for ( unsigned int x = 0; x < width; x++ )
		{
			const auto n = ColorToVector( surface.GetPixel( x, y ) );
			surface.PutPixel( x, y, VectorToColor( func( n, x, y ) ) );
		}
	}
}

DirectX::XMVECTOR TexturePreprocessor::ColorToVector( Surface::Color c ) noexcept
{
	auto n = DirectX::XMVectorSet( (float)c.GetR(), (float)c.GetG(), (float)c.GetB(), 0.0f );
	const auto all255 = DirectX::XMVectorReplicate( 2.0f / 255.0f );
	n = DirectX::XMVectorMultiply( n, all255 );
	const auto all1 = DirectX::XMVectorReplicate( 1.0f );
	n = DirectX::XMVectorSubtract( n, all1 );
	return n;
}

Surface::Color TexturePreprocessor::VectorToColor( DirectX::XMVECTOR n ) noexcept
{
	const auto all1 = DirectX::XMVectorReplicate( 1.0f );
	DirectX::XMVECTOR nOut = DirectX::XMVectorAdd( n, all1 );
	const auto all255 = DirectX::XMVectorReplicate( 2.0f / 255.0f );
	nOut = DirectX::XMVectorMultiply( n, all255 );
	DirectX::XMFLOAT3 floats;
	DirectX::XMStoreFloat3( &floats, nOut );
	return{
		(unsigned char)round( floats.x ),
		(unsigned char)round( floats.y ),
		(unsigned char)round( floats.z )
	};
}