#pragma once

#define GFX_EXCEPT_NOINFO( hr ) //Graphics::HrException( __LINE__, __FILE__, ( hr ) )
#define GFX_THROW_NOINFO( hrcall ) //if( FAILED( hr = hrcall ) ) throw Graphics::HrException( __LINE__, __FILE__, hr )
//#define GFX_THROW_FAILED( hrcall ) if( FAILED( hr == hrcall ) ) throw Graphics::HrException( __LINE__, __FILE__, hr )

#ifndef NDEBUG
#define GFX_EXCEPT( hr ) //Graphics::HrException( __LINE__, __FILE__, ( hr ), infoManager.GetMessages )
#define GFX_THROW_INFO( hrcall ) //infoManager.Set(); if( FAILED( hr = ( hrcall ) ) ) throw GFX_EXCEPT( hr )
#define GFX_DEVICE_REMOVED_EXCEPT( hr ) //Graphics::DeviceRemovedException( __LINE__, __FILE__, ( hr ), infoManager.GetMessages() )
#define GFX_THROW_INFO_ONLY( call ) //infoManager.Set(); { auto v = infoManager.GetMessages(); if ( !v.empty() ) { throw Graphics::InfoException( __LINE__, __FILE__, v ); } }
#else
#define GFX_EXCEPT( hr ) //Graphics::HrException( __LINE__, __FILE__, ( hr ) )
#define GFX_THROW_INFO( hrcall ) //GFX_THROW_NOINFO( hrcall )
#define GFX_DEVICE_REMOVED_EXCEPT( hr ) //Graphics::DeviceRemovedException( __LINE__, __FILE__, ( hr ) )
#define GFX_THROW_INFO_ONLY( call ) //( call )
#endif

// marcro for importing infomanager to local scope
#ifdef NDEBUG
#define INFOMANAGER( gfx ) HRESULT hr
#else
#define INFOMANAGER( gfx ) HRESULT hr;// DxgiInfoManager infoManager = GetInfoManager( ( gfx ) )
#endif

#ifdef NDEBUG
#define INFOMANAGER_NOHR( gfx )
#else
#define INFOMANAGER_NOHR( gfx )// DxgiInfoManager& infoManager = GetInfoManager( ( gfx ) )
#endif