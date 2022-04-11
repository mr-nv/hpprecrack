#include "util.h"
#include "main.h"
#include <thread>

// handler.cpp
void HandleSession( websocket::stream< tcp::socket >* ws, std::string text );

void Session( tcp::socket socket )
{
	try
	{
		websocket::stream< tcp::socket > ws { std::move( socket ) };

		// who cares
		ws.set_option( websocket::stream_base::decorator(
			[ ]( websocket::response_type& res )
		{
			res.set( http::field::server,
				std::string( BOOST_BEAST_VERSION_STRING ) +
				" websocket-server-sync" );
		} ) );

		beast::error_code ec;
		auto remote = ws.next_layer( ).remote_endpoint( ec );

		if( !ec )
			console->DPrintf( "[session] Received connection from [%s]\n", remote.address( ).to_string( ).c_str( ) );
		else
			console->DPrintf( "[session] Failed to get remote -> [%s]\n", ec.message( ).c_str( ) );

		ws.accept( );
		for( ;; )
		{
			beast::flat_buffer buffer;

			ws.read( buffer );

			const auto text = util::GetText( buffer );

			HandleSession( &ws, text );
		}
	}
	catch( const beast::system_error& se )
	{
		if( se.code( ) != websocket::error::closed )
			console->DPrintf( "[session] Got error [%s]\n", se.code( ).message( ).c_str( ) );
	}
	catch( const std::exception& e )
	{
		console->DPrintf( "[session] Got exception [%s]\n", e.what( ) );
	}
}

void CreateServer( )
{
	const auto address = net::ip::make_address( "0.0.0.0" );
	const auto port = static_cast< unsigned short >( 50011 );

	net::io_context ioc { 1 };
	tcp::acceptor acceptor( ioc, { address, port } );

	for( ;; )
	{
		tcp::socket socket { ioc };
		acceptor.accept( socket );

		std::thread( &Session, std::move( socket ) ).detach( );
	}
}

void CreateServerThread( )
{
	const auto thread = CreateThread( 0, 0, ( LPTHREAD_START_ROUTINE )CreateServer, 0, 0, 0 );
	if( thread )
		CloseHandle( ( HANDLE )thread );
}