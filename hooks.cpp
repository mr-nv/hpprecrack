#include "minhook/include/MinHook.h"
#include <iostream>

typedef int( WINAPI* connectFn )( SOCKET socket, const sockaddr* name, int namelen );
connectFn original_connect;

int WINAPI hooked_connect( SOCKET socket, const sockaddr* name, int namelen )
{
	if( !name ) return original_connect( socket, name, namelen );

	sockaddr_in sin;
	memcpy( &sin, name, sizeof( sin ) );

	char addr[ 128 ];
	sprintf_s( addr, "%s:%d", inet_ntoa( sin.sin_addr ), ntohs( sin.sin_port ) );

	// redirect connection
	if( !strcmp( addr, "217.182.72.180:50011" ) )
	{
		sin.sin_addr.S_un.S_addr = inet_addr( "127.0.0.1" );

		return original_connect( socket, ( sockaddr* )&sin, namelen );
	}

	return original_connect( socket, name, namelen );
}

typedef HINSTANCE( WINAPI* ShellExecuteAFn )( HWND, LPCSTR, LPCSTR, LPCSTR, LPCSTR, INT );
ShellExecuteAFn original_ShellExecuteA;

HINSTANCE WINAPI hooked_ShellExecuteA( HWND hwnd, LPCSTR lpOperation, LPCSTR lpFile, LPCSTR lpParameters, LPCSTR lpDirectory, INT nShowCmd )
{
	if( lpOperation && lpFile )
	{
		if( !strcmp( lpOperation, "open" ) && strstr( lpFile, "midnight.im" ) )
			return ( HINSTANCE )1337;
	}

	return original_ShellExecuteA( hwnd, lpOperation, lpFile, lpParameters, lpDirectory, nShowCmd );
}

const char* Hook( )
{
	if( MH_Initialize( ) != MH_OK )
		return "MH_Initialize failed";

	if( MH_CreateHookApi( L"Ws2_32.dll", "connect", hooked_connect, ( LPVOID* )&original_connect ) != MH_OK )
		return "Ws2_32.dll -> connect hook failed";

	if( MH_CreateHookApi( L"Shell32.dll", "ShellExecuteA", hooked_ShellExecuteA, ( LPVOID* )&original_ShellExecuteA ) != MH_OK )
		return "Shell32.dll -> ShellExecuteA hook failed";

	if( MH_EnableHook( MH_ALL_HOOKS ) != MH_OK )
		return "MH_EnableHook failed";

	return nullptr;
}

void Unhook( )
{
	MH_DisableHook( MH_ALL_HOOKS );
	MH_Uninitialize( );
}