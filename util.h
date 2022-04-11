#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>

#include <string>

// i fucking love namespaces
namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

namespace util
{
	std::string GetText( beast::flat_buffer const& buffer );
	void SendText( const std::string text, websocket::stream< tcp::socket >* ws );
	std::string FindArgument( const std::string& sInput, int iNumber, std::string sIterator = ";" );
	bool IsPortInUse( unsigned short port );
	void PrintConsole( const char* text, ... );
	bool Inject( char* bytes, size_t size );
}