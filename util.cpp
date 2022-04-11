#include "util.h"
#include "main.h"

#include <BlackBone/Process/Process.h>
#pragma comment( lib, "BlackBone.lib" )

namespace util
{
	std::string GetText( beast::flat_buffer const& buffer )
	{
		return std::string( boost::asio::buffer_cast< char const* >(
			beast::buffers_front( buffer.data( ) ) ),
			boost::asio::buffer_size( buffer.data( ) ) );
	}

	void SendText( const std::string text, websocket::stream< tcp::socket >* ws )
	{
		ws->text( true );

		net::const_buffer buf( text.c_str( ), text.size( ) );
		ws->write( buf );
	}

	// pasted
	std::string FindArgument( const std::string& sInput, int iNumber, std::string sIterator )
	{
		std::string sTmp = sInput;
		size_t iCount = 0;

		for( auto i = 0; i < iNumber; i++ )
		{
			if( iCount && sTmp.length( ) - iCount > 0 && iCount < sTmp.length( ) - iCount )
				sTmp = sInput.substr( iCount, sTmp.length( ) - iCount );

			auto pos = sTmp.find( sIterator );

			if( pos == std::string::npos )
				return std::string( );

			sTmp = sTmp.substr( 0, pos );

			iCount += sTmp.length( ) + 1;
		}

		return sTmp;
	}

	bool IsPortInUse( unsigned short port )
	{
		boost::asio::io_service service;
		tcp::acceptor acceptor( service );

		boost::system::error_code code;
		acceptor.open( tcp::v4( ), code );
		acceptor.bind( { tcp::v4( ), port }, code );

		bool closed = code == boost::asio::error::address_in_use;

		if( acceptor.is_open( ) )
			acceptor.close( );

		return closed;
	}

	void PrintConsole( const char* text, ... )
	{
		if( !DEBUG ) return;

		char converted[ 1024 ];
		va_list list;
		va_start( list, text );
		vsnprintf( converted, sizeof( converted ), text, list );
		va_end( list );

		console->DPrintf( converted );
	}

	bool Inject( char* bytes, size_t size )
	{
		blackbone::Process process;
		if( !NT_SUCCESS( process.Attach( GetCurrentProcess( ) ) ) )
			return false;

		// https://github.com/DarthTon/Blackbone/issues/240#issuecomment-384822878
		auto buf = VirtualAlloc( 0, size, MEM_COMMIT, PAGE_READWRITE );
		memcpy( buf, bytes, size );

		const auto result = process.mmap( ).MapImage( size, buf, false,
			blackbone::CreateLdrRef | blackbone::RebaseProcess | blackbone::NoDelayLoad | blackbone::NoTLS );

		process.Detach( );

		return result.success( );
	}
}