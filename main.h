#pragma once
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>

#include <string>
#include <filesystem>
#include <io.h>
#include <algorithm>

#include "IGameConsole.h"

// i fucking love namespaces
namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

extern IGameConsole* console;

constexpr bool DEBUG = false;