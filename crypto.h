#pragma once
#include <Windows.h>
#include <string>

namespace crypto
{
	void Xor( std::string& str );
	void Base64Encode( std::string& str );
	void Base64Decode( std::string& str );
	std::string HexToString( const std::string& in );

	void EncryptServerHWIDResponse( std::string& str );
	void DecryptClientHWIDResponse( std::string& str );
}