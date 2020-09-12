#include "Surface.h"
#include "Window.h"
#include "StringConverter.h"
#include <algorithm>
#include <cassert>
#include <sstream>
#include <filesystem>

Surface::Surface( unsigned int width, unsigned int height )
{
	HRESULT hr = scratch.Initialize2D( format, width, height, 1u, 1u );
	if ( FAILED( hr ) )
		throw Surface::SurfaceException( __LINE__, __FILE__, "Failed to initialize ScratchImage!", hr );
}

void Surface::Clear( Color fillValue ) noexcept
{
	const auto width = GetWidth();
	const auto height = GetHeight();
	auto& imageData = *scratch.GetImage( 0, 0, 0 );
	for ( size_t y = 0u; y < height; y++ )
	{
		auto rowStart = reinterpret_cast<Color*>( imageData.pixels + imageData.rowPitch * y );
		std::fill( rowStart, rowStart + imageData.width, fillValue );
	}
}

void Surface::PutPixel( unsigned int x, unsigned int y, Color c ) noexcept(!IS_DEBUG)
{
	assert( x >= 0 );
	assert( y >= 0 );
	assert( x < GetWidth() );
	assert( y < GetHeight() );
	auto& imageData = *scratch.GetImage( 0, 0, 0 );
	reinterpret_cast<Color*>( &imageData.pixels[y * imageData.rowPitch] )[x] = c;
}

Surface::Color Surface::GetPixel( unsigned int x, unsigned int y ) const noexcept(!IS_DEBUG)
{
	assert( x >= 0 );
	assert( y >= 0 );
	assert( x < GetWidth() );
	assert( y < GetHeight() );
	auto& imageData = *scratch.GetImage( 0, 0, 0 );
	return reinterpret_cast<Color*>( &imageData.pixels[y * imageData.rowPitch] )[x];
}

unsigned int Surface::GetWidth() const noexcept
{
	return static_cast<unsigned int>( scratch.GetMetadata().width );
}

unsigned int Surface::GetHeight() const noexcept
{
	return static_cast<unsigned int>( scratch.GetMetadata().height );
}

unsigned int Surface::GetBytePitch() const noexcept
{
	return static_cast<unsigned int>( scratch.GetImage( 0, 0, 0 )->rowPitch );
}

Surface::Color* Surface::GetBufferPtr() noexcept
{
	return reinterpret_cast<Color*>( scratch.GetPixels() );
}

const Surface::Color* Surface::GetBufferPtr() const noexcept
{
	return const_cast<Surface*>( this )->GetBufferPtr();
}

const Surface::Color* Surface::GetBufferPtrConst() const noexcept
{
	return const_cast<Surface*>( this )->GetBufferPtr();
}

Surface Surface::FromFile( const std::string& name )
{
	DirectX::ScratchImage scratch;
	HRESULT hr = DirectX::LoadFromWICFile( ToWide( name ).c_str(), DirectX::WIC_FLAGS_IGNORE_SRGB, nullptr, scratch );

	if ( FAILED( hr ) )
		throw Surface::SurfaceException( __LINE__, __FILE__, "Failed to load scratch image!", hr );

	if ( scratch.GetImage( 0, 0, 0 )->format != format )
	{
		DirectX::ScratchImage converted;
		hr = DirectX::Convert(
			*scratch.GetImage( 0, 0, 0 ),
			format,
			DirectX::TEX_FILTER_DEFAULT,
			DirectX::TEX_THRESHOLD_DEFAULT,
			converted
		);

		if ( FAILED( hr ) )
			throw Surface::SurfaceException( __LINE__, __FILE__, "Failed to convert scratch image!", hr );

		return Surface( std::move( converted ) );
	}

	return Surface( std::move( scratch ) );
}

void Surface::Save( const std::string& filename ) const
{
	const auto GetCodecID = []( const std::string& filename )
	{
		const std::filesystem::path path = filename;
		const auto ext = path.extension().string();
		if ( ext == ".png" )
		{
			return DirectX::WIC_CODEC_PNG;
		}
		else if ( ext == ".jpg" )
		{
			return DirectX::WIC_CODEC_JPEG;
		}
		else if ( ext == ".bmp" )
		{
			return DirectX::WIC_CODEC_BMP;
		}
		throw Surface::SurfaceException( __LINE__, __FILE__, "Image format not supported!" );
	};

	HRESULT hr = DirectX::SaveToWICFile(
		*scratch.GetImage( 0, 0, 0 ),
		DirectX::WIC_FLAGS_NONE,
		GetWICCodec( GetCodecID( filename ) ),
		ToWide( filename ).c_str()
	);

	if ( FAILED( hr ) )
		throw Surface::SurfaceException( __LINE__, __FILE__, "Failed to save scratch image!", hr );
}

bool Surface::AlphaLoaded() const noexcept
{
	return !scratch.IsAlphaAllOpaque();
}

Surface::Surface( DirectX::ScratchImage scratch ) noexcept
	:
	scratch( std::move( scratch ) )
{}


// surface exception stuff
Surface::SurfaceException::SurfaceException( int line, const char* file, std::string note, std::optional<HRESULT> hr ) noexcept
	:
	Exception( line, file ),
	note( "[Note]" + note )
{
	if ( hr )
		note = "[Error String]" + Window::WindowException::TranslateErrorCode( *hr ) + "\n" + note;
}

const char* Surface::SurfaceException::what() const noexcept
{
	std::ostringstream oss;
	oss << Exception::what() << std::endl << GetNote();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Surface::SurfaceException::GetType() const noexcept
{
	return "Surface Exception";
}

const std::string& Surface::SurfaceException::GetNote() const noexcept
{
	return note;
}