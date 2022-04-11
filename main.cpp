#define _CRT_SECURE_NO_WARNINGS
#include "main.h"
#include "util.h"

HMODULE mod = nullptr;

IGameConsole* console = nullptr;

// server.cpp
void CreateServerThread( );

// hooks.cpp
const char* Hook( );
void Unhook( );

#include <vector>
#include <fstream>
#include "crypto.h"

void Start( )
{
	const auto CaptureFactory = ( CreateInterfaceFn )GetProcAddress( GetModuleHandleA( "gameui.dll" ), "CreateInterface" );
	if( !CaptureFactory ) return FreeLibraryAndExitThread( mod, 0 );

	console = ( IGameConsole* )( CaptureFactory( GAMECONSOLE_INTERFACE_VERSION, 0 ) );
	if( !console ) return FreeLibraryAndExitThread( mod, 0 );

	if( !console->IsConsoleVisible( ) )
		console->Activate( );

	console->DPrintf( "hpp.ovh crack offline server emulator\n" );
	console->DPrintf( "by mrnv @ t.me/ayywareseller\n" );

	if( util::IsPortInUse( 50011 ) )
	{
		console->DPrintf( "Port 50011 is in use, can't continue\n" );
		FreeLibraryAndExitThread( mod, 0 );
		return;
	}

	console->DPrintf( "Creating local server at port 50011\n" );
	CreateServerThread( );
	Sleep( 1500 );

	util::PrintConsole( "Hooking functions\n" );
	const auto hooks = Hook( );
	if( hooks )
	{
		console->DPrintf( "Hooking failed [%s]\n", hooks );
		FreeLibraryAndExitThread( mod, 0 );
		return;
	}

	const auto res = FindResourceW( mod, MAKEINTRESOURCEW( 101 ), L"TEXT" );
	if( !res )
	{
		console->DPrintf( "Failed to find hpp.dll in resources\n" );
		Unhook( );
		FreeLibraryAndExitThread( mod, 0 );
		return;
	}

	const auto reshandle = LoadResource( mod, res );
	if( !reshandle )
	{
		console->DPrintf( "Failed to load hpp.dll from resources\n" );
		Unhook( );
		FreeLibraryAndExitThread( mod, 0 );
		return;
	}

	const auto ressize = SizeofResource( mod, res );
	const auto resdata = ( char* )LockResource( reshandle );

	console->DPrintf( "Injecting original DLL\n" );

	if( !util::Inject( resdata, ressize ) )
	{
		console->DPrintf( "Failed to inject original DLL\n" );
		Unhook( );
		FreeLibraryAndExitThread( mod, 0 );
	}

	console->DPrintf( "Done\n" );
}

BOOL WINAPI DllMain( HINSTANCE instance, DWORD reason, void* reserved )
{
	if( reason == DLL_PROCESS_ATTACH )
	{
		mod = instance;
		const auto thread = CreateThread( 0, 0, ( LPTHREAD_START_ROUTINE )Start, reserved, 0, 0 );
		if( thread )
		{
			CloseHandle( ( HANDLE )thread );
			return TRUE;
		}
	}

	return FALSE;
}