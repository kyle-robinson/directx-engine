#include "Material.h"
#include "Stencil.h"
#include "BindableCommon.h"
#include "DynamicConstant.h"
#include "ConstantBufferEx.h"
#include "TransformCbufScaling.h"

Material::Material( Graphics& gfx, const aiMaterial& material, const std::filesystem::path& path ) noexcept(!IS_DEBUG)
	: modelPath( path.string() )
{
	const auto rootPath = path.parent_path().string() + "\\";
	{
		aiString tempName;
		material.Get( AI_MATKEY_NAME, tempName );
		name = tempName.C_Str();
	}
	// phong
	{
		Technique phong{ "Phong" };
		Step step( "lambertian" );
		std::string shaderCode = "Phong";
		aiString texFileName;

		// common - pre
		layout.Append( VertexMeta::VertexLayout::Position3D );
		layout.Append( VertexMeta::VertexLayout::Normal );
		Dcb::RawLayout rawLayout;
		bool hasTexture = false;
		bool hasGlossAlpha = false;

		// diffuse
		{
			bool hasAlpha = false;
			if ( material.GetTexture( aiTextureType_DIFFUSE, 0, &texFileName ) == aiReturn_SUCCESS )
			{
				hasTexture = true;
				shaderCode += "Dif";
				layout.Append( VertexMeta::VertexLayout::Texture2D );
				auto tex = Bind::Texture::Resolve( gfx, rootPath + texFileName.C_Str() );
				if ( tex->HasAlpha() )
				{
					hasAlpha = true;
					shaderCode += "Msk";
				}
				step.AddBindable( std::move( tex ) );
			}
			else
			{
				rawLayout.Add<Dcb::Float3>( "materialColor" );
			}
			step.AddBindable( Bind::Rasterizer::Resolve( gfx, hasAlpha ) );
		}
		// specular
		{
			if ( material.GetTexture( aiTextureType_SPECULAR, 0, &texFileName ) == aiReturn_SUCCESS )
			{
				hasTexture = true;
				shaderCode += "Spc";
				layout.Append( VertexMeta::VertexLayout::Texture2D );
				auto tex = Bind::Texture::Resolve( gfx, rootPath + texFileName.C_Str(), 1u );
				hasGlossAlpha = tex->HasAlpha();
				step.AddBindable( std::move( tex ) );
				rawLayout.Add<Dcb::Bool>( "useGlossAlpha" );
				rawLayout.Add<Dcb::Bool>( "useSpecularMap" );
			}
			rawLayout.Add<Dcb::Float3>( "specularColor" );
			rawLayout.Add<Dcb::Float>( "specularWeight" );
			rawLayout.Add<Dcb::Float>( "specularGloss" );
		}
		// normal
		{
			if ( material.GetTexture( aiTextureType_NORMALS, 0, &texFileName ) == aiReturn_SUCCESS )
			{
				hasTexture = true;
				shaderCode += "Nrm";
				layout.Append( VertexMeta::VertexLayout::Texture2D );
				layout.Append( VertexMeta::VertexLayout::Tangent );
				layout.Append( VertexMeta::VertexLayout::Bitangent );
				step.AddBindable( Bind::Texture::Resolve( gfx, rootPath + texFileName.C_Str(), 2u ) );
				rawLayout.Add<Dcb::Bool>( "useNormalMap" );
				rawLayout.Add<Dcb::Float>( "normalMapWeight" );
			}
		}
		// common - post
		{
			step.AddBindable( std::make_shared<Bind::TransformCbuf>( gfx, 0u ) );
			step.AddBindable( Bind::Blender::Resolve( gfx, false ) );
			auto pvs = Bind::VertexShader::Resolve( gfx, shaderCode + "VS.cso" );
			auto pvsbc = pvs->GetByteCode();
			step.AddBindable( std::move( pvs ) );
			step.AddBindable( Bind::PixelShader::Resolve( gfx, shaderCode + "PS.cso" ) );
			step.AddBindable( Bind::InputLayout::Resolve( gfx, layout, pvsbc ) );
			if ( hasTexture )
				step.AddBindable( Bind::Sampler::Resolve( gfx ) );
			// PS Material CBuf
			Dcb::Buffer buf{ std::move( rawLayout ) };
			if ( auto r = buf["materialColor"]; r.Exists() )
			{
				aiColor3D color = { 0.45f, 0.45f, 0.85f };
				material.Get( AI_MATKEY_COLOR_DIFFUSE, color );
				r = reinterpret_cast<DirectX::XMFLOAT3&>( color );
			}
			buf["useGlossAlpha"].SetIfExists( hasGlossAlpha );
			buf["useSpecularMap"].SetIfExists( true );
			if ( auto r = buf["specularColor"]; r.Exists() )
			{
				aiColor3D color = { 0.18f, 0.18f, 0.18f };
				material.Get( AI_MATKEY_COLOR_SPECULAR, color );
				r = reinterpret_cast<DirectX::XMFLOAT3&>( color );
			}
			buf["specularWeight"].SetIfExists( 1.0f );
			if ( auto r = buf["specularGloss"]; r.Exists() )
			{
				float gloss = 8.0f;
				material.Get( AI_MATKEY_SHININESS, gloss );
				r = gloss;
			}
			buf["useNormalMap"].SetIfExists( true );
			buf["normalMapWeight"].SetIfExists( 1.0f );
			step.AddBindable( std::make_unique<Bind::CachingPixelConstantBufferEx>( gfx, std::move( buf ), 1u ) );
		}
		phong.AddStep( std::move( step ) );
		techniques.push_back( std::move( phong ) );
	}
	// outline
	{
		Technique outline( "Outline", false );
		{
			Step mask( "outlineMask" );

			mask.AddBindable( Bind::InputLayout::Resolve( gfx, layout, Bind::VertexShader::Resolve( gfx, "SolidVS.cso" )->GetByteCode() ) );
			mask.AddBindable( std::make_shared<Bind::TransformCbuf>( gfx ) );

			outline.AddStep( std::move( mask ) );
		}
		{
			Step draw( "outlineDraw" );

			{
				Dcb::RawLayout lay;
				lay.Add<Dcb::Float3>( "materialColor" );
				auto buf = Dcb::Buffer( std::move( lay ) );
				buf["materialColor"] = DirectX::XMFLOAT3{ 1.0f, 0.4f, 0.4f };
				draw.AddBindable( std::make_shared<Bind::CachingPixelConstantBufferEx>( gfx, buf, 1u ) );
			}

			draw.AddBindable( Bind::InputLayout::Resolve( gfx, layout, Bind::VertexShader::Resolve( gfx, "SolidVS.cso" )->GetByteCode() ) );

			draw.AddBindable( std::make_shared<Bind::TransformCbufScaling>( gfx, 1.04f ) );

			outline.AddStep( std::move( draw ) );
		}
		techniques.push_back( std::move( outline ) );
	}
}

VertexMeta::VertexBuffer Material::ExtractVertices( const aiMesh& mesh ) const noexcept
{
	return { layout, mesh };
}

std::vector<unsigned short> Material::ExtractIndices( const aiMesh& mesh ) const noexcept
{
	std::vector<unsigned short> indices;
	indices.reserve(mesh.mNumFaces * 3);
	for (unsigned int i = 0; i < mesh.mNumFaces; i++)
	{
		const auto& face = mesh.mFaces[i];
		assert(face.mNumIndices == 3);
		indices.push_back(face.mIndices[0]);
		indices.push_back(face.mIndices[1]);
		indices.push_back(face.mIndices[2]);
	}
	return indices;
}

std::shared_ptr<Bind::VertexBuffer> Material::MakeVertexBindable(Graphics& gfx, const aiMesh& mesh, float scale) const noexcept(!IS_DEBUG)
{
	auto vtx = ExtractVertices( mesh );
	if ( scale != 1.0f )
	{
		for ( auto i = 0; i < vtx.Size(); i++ )
		{
			DirectX::XMFLOAT3& pos = vtx[i].Attr<VertexMeta::VertexLayout::ElementType::Position3D>();
			pos.x *= scale;
			pos.y *= scale;
			pos.z *= scale;
		}
	}
	return Bind::VertexBuffer::Resolve(gfx, MakeMeshTag(mesh), std::move(vtx));
}

std::shared_ptr<Bind::IndexBuffer> Material::MakeIndexBindable(Graphics& gfx, const aiMesh& mesh) const noexcept(!IS_DEBUG)
{
	return Bind::IndexBuffer::Resolve(gfx, MakeMeshTag(mesh), ExtractIndices(mesh));
}

std::vector<Technique> Material::GetTechniques() const noexcept
{
	return techniques;
}

std::string Material::MakeMeshTag(const aiMesh& mesh) const noexcept
{
	return modelPath + "%" + mesh.mName.C_Str();
}