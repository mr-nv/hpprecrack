#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>

#include <string>

#include "util.h"
#include "crypto.h"
#include "main.h"

// i fucking love namespaces
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

void HandleSession( websocket::stream< tcp::socket >* ws, std::string text )
{
	if( text._Starts_with( "c2s_hwid:" ) )
	{
		util::PrintConsole( "Received c2s_hwid request\n" );
		crypto::DecryptClientHWIDResponse( text );

		std::string response = "";

		response += util::FindArgument( text, 1 );
		response += ";";
		response += util::FindArgument( text, 2 );
		response += ";";
		response += util::FindArgument( text, 3 );
		response += ";1;";
		response += text.substr( response.length( ) - 2 );

		crypto::EncryptServerHWIDResponse( response );

		response = "s2c_hwid:" + response;

		util::PrintConsole( "Sending [%s]\n", response.c_str( ) );
		util::SendText( response, ws );
	}
	else if( text._Starts_with( "c2s_online:" ) )
	{
		std::string response = "s2c_online:1337";
		util::PrintConsole( "Sending [%s]\n", response.c_str( ) );
		util::SendText( response, ws );
	}
	else
	{
		if( !text._Starts_with( "c2s_steamid" ) )
			util::PrintConsole( "Received unknown text [%s]\n", text.c_str( ) );
	}
}