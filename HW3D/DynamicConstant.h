#pragma once
#include <DirectXMath.h>
#include <cassert>
#include <string>
#include <vector>
#include <memory>
#include <optional>

#define LEAF_ELEMENT_TYPES \
	X( Float ) \
	X( Float2 ) \
	X( Float3 ) \
	X( Float4 ) \
	X( Matrix ) \
	X( Bool ) \
	X( Integer )

namespace Dcb
{
	enum Type
	{
#define X( element ) element,
		LEAF_ELEMENT_TYPES
#undef X
		Struct,
		Array,
		Empty
	};

	// static map of the attributes of each leaf type
	template<Type type>
	struct Map
	{
		static constexpr bool valid = false;
	};
	template<> struct Map<Float>
	{
		using DynamicType = float; // type used on CPU
		static constexpr size_t hlslSize = sizeof( DynamicType ); // size of type on GPU for alignment purposes
		static constexpr const char* code = "F1"; // code used to generate signature of layout
		static constexpr bool valid = true; // meta programming flag used to check the validity of Map <param>
	};
	template<> struct Map<Float2>
	{
		using DynamicType = DirectX::XMFLOAT2;
		static constexpr size_t hlslSize = sizeof( DynamicType );
		static constexpr const char* code = "F2";
		static constexpr bool valid = true;
	};
	template<> struct Map<Float3>
	{
		using DynamicType = DirectX::XMFLOAT3;
		static constexpr size_t hlslSize = sizeof( DynamicType );
		static constexpr const char* code = "F3";
		static constexpr bool valid = true;
	};
	template<> struct Map<Float4>
	{
		using DynamicType = DirectX::XMFLOAT4;
		static constexpr size_t hlslSize = sizeof( DynamicType );
		static constexpr const char* code = "F4";
		static constexpr bool valid = true;
	};
	template<> struct Map<Matrix>
	{
		using DynamicType = DirectX::XMFLOAT4X4;
		static constexpr size_t hlslSize = sizeof( DynamicType );
		static constexpr const char* code = "M4";
		static constexpr bool valid = true;
	};
	template<> struct Map<Bool>
	{
		using DynamicType = bool;
		static constexpr size_t hlslSize = 4u;
		static constexpr const char* code = "BL";
		static constexpr bool valid = true;
	};
	template<> struct Map<Integer>
	{
		using DynamicType = int;
		static constexpr size_t hlslSize = sizeof( DynamicType );
		static constexpr const char* code = "IN";
		static constexpr bool valid = true;
	};

	// checks that every leaf type has entry in static attribute map
#define X( element ) static_assert( Map<element>::valid, "Missing map implementation for " #element );
	LEAF_ELEMENT_TYPES
#undef X

	// enables reverse lookup of leaf elements
	template<typename T>
	struct ReverseMap
	{
		static constexpr bool valid = false;
	};
#define X( element ) \
	template<> struct ReverseMap<typename Map<element>::DynamicType> \
	{ \
		static constexpr Type type = element; \
		static constexpr bool valid = true; \
	};
	LEAF_ELEMENT_TYPES
#undef x

	// instances form a tree that describes the layout of the data buffer
	class LayoutElement
	{
	private:
		// forms polymorphic base for extra data that Sturct and Array have
		struct ExtraDataBase
		{
			virtual ~ExtraDataBase() = default;
		};
		friend class RawLayout;
		friend struct ExtraData;
	public:
		std::string GetSignature() const noexcept(!IS_DEBUG);
		bool Exists() const noexcept;
		// calculate the array indexing offset
		std::pair<size_t, const LayoutElement*> CalculateIndexingOffset( size_t offset, size_t index ) const noexcept(!IS_DEBUG);
		
		// [] only works for structs - access member child nodes in tree by name
		LayoutElement& operator[]( const std::string& key ) noexcept(!IS_DEBUG);
		const LayoutElement& operator[]( const std::string& key ) const noexcept(!IS_DEBUG);
		
		// T() only works for arrays - gets the array type layout
		LayoutElement& T() noexcept(!IS_DEBUG);
		const LayoutElement& T() const noexcept(!IS_DEBUG);
		
		// offset-based functions
		size_t GetOffsetBegin() const noexcept(!IS_DEBUG);
		size_t GetOffsetEnd() const noexcept(!IS_DEBUG);
		// get size in bytes derived from offsets
		size_t GetSizeInBytes() const noexcept(!IS_DEBUG);

		// only works for structs - add LayoutElement to struct
		LayoutElement& Add( Type addedType, std::string name ) noexcept(!IS_DEBUG);
		template<Type typeAdded>
		LayoutElement& Add( std::string key ) noexcept(!IS_DEBUG)
		{
			return Add( typeAdded, std::move( key ) );
		}

		// only works for arrays - set the type and number of elements
		LayoutElement& Set( Type addedType, size_t size ) noexcept(!IS_DEBUG);
		template<Type typeAdded>
		LayoutElement& Set( size_t size ) noexcept(!IS_DEBUG)
		{
			Set( typeAdded, size );
		}

		// returns offset of leaf type for read/write
		template<typename T>
		size_t Resolve() const noexcept(!IS_DEBUG)
		{
			switch ( type )
			{
#define X( element ) case element: assert( typeid( Map<element>::DynamicType ) == typeid( T ) ); return *offset;
			LEAF_ELEMENT_TYPES
#undef X
			default:
				assert( "Tried to resolve non-leaf element!" && false );
				return 0u;
			}
		}
	private:
		// construct empty LayoutElement
		LayoutElement() noexcept = default;
		LayoutElement( Type typeIn ) noexcept(!IS_DEBUG);

		// sets all offsets for element and subelements - padding is added where necessary
		size_t Finalize( size_t offsetIn ) noexcept(!IS_DEBUG);

		// implementations for GetSignature
		std::string GetSignatureForStruct() const noexcept(!IS_DEBUG);
		std::string GetSignatureForArray() const noexcept(!IS_DEBUG);

		// implementations for Finalize
		size_t FinalizeForStruct( size_t offsetIn );
		size_t FinalizeForArray( size_t offsetIn );

		// returns singleton instance of empty layout element
		static LayoutElement& GetEmptyElement() noexcept
		{
			static LayoutElement empty{};
			return empty;
		}

		// returns value of offset up to next 16-byte boundary
		static size_t AdvanceToBoundary( size_t offset ) noexcept;
		// return true if memory block crosses boundary
		static bool CrossesBoundary( size_t offset, size_t size ) noexcept;
		// advance an offset to next boundary if block crosses
		static size_t AdvanceIfCrossesBoundary( size_t offset, size_t size ) noexcept;
		// check string for validity as a struct key
		static bool ValidateSymbolName( const std::string& name ) noexcept;
	private:
		// each element stores its own offset - makes lookup into byte buffer faster
		Type type = Empty;
		std::optional<size_t> offset;
		std::unique_ptr<ExtraDataBase> pExtraData;
	};

	// 1 - acts as a shell to hold the root of the LayoutElement tree
	// 2 - create RawLayout and access and add elements from there
	// 3 - when layout is complete, move to codex to finalize
	// 4 - buffer uses the returned layout from codex to initialize itself
	class Layout
	{
		friend class LayoutCodex;
		friend class Buffer;
	public:
		size_t GetSizeInBytes() const noexcept;
		std::string GetSignature() const noexcept(!IS_DEBUG);
	protected:
		Layout( std::shared_ptr<LayoutElement> pRoot ) noexcept;
		std::shared_ptr<LayoutElement> pRoot;
	};

	// RawLayout is a layout that has not yet been finalized
	class RawLayout : public Layout
	{
		friend class LayoutCodex;
	public:
		RawLayout() noexcept;
		// key into the root struct
		LayoutElement& operator[]( const std::string& key ) noexcept(!IS_DEBUG);
		// add element to root struct
		template<Type type>
		LayoutElement& Add( const std::string& key ) noexcept(!IS_DEBUG)
		{
			return pRoot->Add<type>( key );
		}
	private:
		// reset object with empty struct as the root
		void ClearRoot() noexcept;
		// finalize layout and the relinquish
		std::shared_ptr<LayoutElement> DeliverRoot() noexcept;
	};

	// CompleteLayout represents a completed and registered shell
	class CompleteLayout : public Layout
	{
		friend class LayoutCodex;
		friend class Buffer;
	public:
		// key into the root struct
		const LayoutElement& operator[]( const std::string& key ) const noexcept(!IS_DEBUG);
		// get a share on the layout tree root
		std::shared_ptr<LayoutElement> ShareRoot() const noexcept;
	private:
		// used by Codex to return complete layout
		CompleteLayout( std::shared_ptr<LayoutElement> pRoot ) noexcept;
		// used to pilfer the layout tree
		std::shared_ptr<LayoutElement> RelinquishRoot() const noexcept;
	};

	// proxy type used when indexing into a buffer element allowing for manipulation of raw bytes of the buffer
	class ConstElementRef
	{
		friend class Buffer;
		friend class ElementRef;
	public:
		// proxy used when using addressof& on the Ref - allows for conversion to pointer type
		class Pointer
		{
			friend class ConstElementRef;
		public:
			// conversion for getting read-only pointer to supported DynamicType
			template<typename T>
			operator const T* () const noexcept(!IS_DEBUG)
			{
				static_assert( ReverseMap<std::remove_const_t<T>>::valid, "Unsupported DynamicType used in pointer conversion!" );
				return &static_cast<const T&>( &ref );
			}
		private:
			Pointer( const ConstElementRef* ref ) noexcept;
			const ConstElementRef* ref;
		};
	public:
		// check if the indexed element exists
		bool Exists() const noexcept;
		
		// index into current element as a struct
		ConstElementRef operator[]( const std::string& key ) const noexcept(!IS_DEBUG);
		// index into current element as array
		ConstElementRef operator[]( size_t index ) const noexcept(!IS_DEBUG);

		// emit pointer proxy object
		Pointer operator&() const noexcept(!IS_DEBUG);
		// conversion for reading as supported DynamicType
		template<typename T>
		operator const T& () const noexcept(!IS_DEBUG)
		{
			static_assert( ReverseMap<std::remove_const_t<T>>::valid, "Unsupported DynamicType used in conversion!" );
			return *reinterpret_cast<const T*>( pBytes + offset + pLayout->Resolve<T>() );
		}
	private:
		// refs can only be constructed by other refs or the buffer
		ConstElementRef( const LayoutElement* pLayout, const char* pBytes, size_t offset ) noexcept;
		// offset built up by indexing into arrays for every array index in the path of access to the struct
		size_t offset;
		const char* pBytes;
		const LayoutElement* pLayout;
	};

	// copy of ConstElementRef that allows for writing to the bytes of Buffer
	class ElementRef
	{
		friend class Buffer;
	public:
		class Pointer
		{
			friend class ElementRef;
		public:
			// conversion to read/write pointer from supported DynamicType
			template<typename T>
			operator T* () const noexcept(!IS_DEBUG)
			{
				static_assert( ReverseMap<std::remove_const_t<T>>::valid, "Unsupported DynamicType used in pointer conversion!" );
				return &static_cast<T&>( *ref );
			}
		private:
			Pointer( ElementRef* ref ) noexcept;
			ElementRef* ref;
		};
	public:
		operator ConstElementRef() const noexcept;
		bool Exists() const noexcept;

		ElementRef operator[]( const std::string& key ) const noexcept(!IS_DEBUG);
		ElementRef operator[]( size_t index ) const noexcept(!IS_DEBUG);

		// optionally set value if not empty ref
		template<typename S>
		bool SetIfExists( const S& value ) noexcept(!IS_DEBUG)
		{
			if ( Exists() )
			{
				*this = value;
				return true;
			}
			return false;
		}

		Pointer operator&() const noexcept(!IS_DEBUG);
		// conversion for read/write as supported DynamicType
		template<typename T>
		operator T& () const noexcept(!IS_DEBUG)
		{
			static_assert( ReverseMap<std::remove_const_t<T>>::valid, "Unsupported DynamicType used in conversion!" );
			return *reinterpret_cast<T*>( pBytes + offset + pLayout->Resolve<T>() );
		}
		// assignment for writing to as supported DynamicType
		template<typename T>
		T& operator=( const T& rhs ) const noexcept(!IS_DEBUG)
		{
			static_assert( ReverseMap<std::remove_const_t<T>>::valid, "Unsupported DynamicType used in assignment!" );
			return static_cast<T&>( *this ) = rhs;
		}
	private:
		// refs can only be constructed by other refs or the Buffer
		ElementRef( const LayoutElement* pLayout, char* pBytes, size_t offset ) noexcept;
		char* pBytes;
		size_t offset;
		const LayoutElement* pLayout;
	};

	// 1 - Buffer is a combination of a raw byte buffer with a LayoutElement tree structure
	// 2 - used as a view/interpret/overlay for those bytes
	// 3 - operator[] indexes into the root struct, returning ref shell used for further indexing
	// 4 - this can be used to return further ref shells, or to access the data stored int the buffer if leaf element type
	class Buffer
	{
	public:
		// resources to be used to construct a Buffer
		Buffer( RawLayout&& layout ) noexcept(!IS_DEBUG);
		Buffer( const CompleteLayout& layout ) noexcept(!IS_DEBUG);
		Buffer( CompleteLayout&& layout ) noexcept(!IS_DEBUG);
		Buffer( const Buffer& ) noexcept;
		// the buffer that has been pilfered must not be used
		Buffer( Buffer&& ) noexcept;

		// begin indexing into buffer
		ElementRef operator[]( const std::string& key ) noexcept(!IS_DEBUG);
		ConstElementRef operator[]( const std::string& key ) const noexcept(!IS_DEBUG);

		// get the raw bytes
		const char* GetData() const noexcept;
		// get size of raw byte buffer
		size_t GetSizeInBytes() const noexcept;
		const LayoutElement& GetRootLayoutElement() const noexcept;
		// copy bytes from another buffer
		void CopyFrom( const Buffer& ) noexcept(!IS_DEBUG);
		// return another shared_ptr to the layout root
		std::shared_ptr<LayoutElement> ShareLayoutRoot() const noexcept;
	private:
		std::shared_ptr<LayoutElement> pLayoutRoot;
		std::vector<char> bytes;
	};
}

#ifndef DCB_IMPL_SOURCE
	#undef LEAF_ELEMENT_TYPES
#endif