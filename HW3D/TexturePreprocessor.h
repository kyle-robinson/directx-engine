#pragma once
#include "Surface.h"
#include <string>
#include <DirectXMath.h>

class TexturePreprocessor
{
public:
	static void FlipAllYNormalsInObj( const std::string& path );
	static void FlipYNormalMap( const std::string& pathIn, const std::string& pathOut );
	static void ValidateNormalMap( const std::string& pathIn, float thresholdMin, float thresholdMax );
private:
	template<typename F>
	static void TransformFile( const std::string& pathIn, const std::string& pathOut, F&& func );
	template<typename F>
	static void TransformSurface( Surface& surface, F&& func );
	static DirectX::XMVECTOR ColorToVector( Surface::Color c ) noexcept;
	static Surface::Color VectorToColor( DirectX::XMVECTOR n ) noexcept;
};