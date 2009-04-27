/*
 * main.c
 *
 *  Created on: Dec 9, 2008
 *      Author: jasonsantos
 */

#include <windows.h>
#include <wininet.h>
#include <stdlib.h>



#include "lua/lua.h"
#include "lua/lauxlib.h"
#include "lua/lualib.h"


HWND window;

char const * INIT_FILE = "\\usr\\local\\share\\lua\\5.1\\init.lua";
wchar_t const * WINIT_FILE = L"\\usr\\local\\share\\lua\\5.1\\init.lua";

const wchar_t *_T(const char*str) {
	// Special case of empty string
	if (!str[0]) {
		return L"" ;
	}

	// Get the required number of WCHARs for destination string
	int requiredWChars = MultiByteToWideChar(
			CP_ACP, // ANSI code page
			0, // Default flags
			str, // String to be converted
			-1, // Process whole string till NUL terminator
			NULL, // Destination buffer unused
			0 // Request number of WCHARs
	);

	if ( requiredWChars == 0 )
	return L""; // *** ERROR

	// Allocate buffer for Unicode string
	wchar_t* unicodeBuffer = (wchar_t*)malloc( requiredWChars );

	// Convert from ANSI to Unicode

	if (MultiByteToWideChar(
					CP_ACP, // ANSI code page
					0, // Default flags
					str, // String to be converted
					-1, // Process whole string till NUL terminator
					unicodeBuffer, // Destination buffer
					requiredWChars // Destination buffer size
			) == 0 )

	return L""; // *** ERROR

	// Return result string
	return unicodeBuffer;

}


char* wordToMultibyte(wchar_t* wstr, char* out) {
	wchar_t					wout[BUFSIZ] = L"";

	WideCharToMultiByte(CP_ACP, (DWORD)NULL, wout, -1, out, BUFSIZ, NULL, NULL);
	return out;
}


int lua_panic(lua_State*L) {
	MessageBoxW(window, _T(lua_tostring(L, -1)), _T("Lua PANIC"), 0);
	return 0;
}

/*
 const wchar_t *_T(const char*mbs) {
 int wclen = (mblen(mbs, MB_CUR_MAX)+1)*2;

 wchar_t* wcstr = malloc(wclen);
 memset(wcstr, 0, wclen);

 mbstowcs(wcstr, mbs, mblen(mbs, MB_CUR_MAX));

 return wcstr;
 }
 */
int message(lua_State*L) {
	luaL_checkany(L, 1);
	MessageBoxW(window, _T(lua_tostring(L, 1)), _T("Lua 5.1"), 0);
	return 0;
}

int isconnected(lua_State*L) {
	BOOL  BSuccess;
	DWORD DWFlags;

	BSuccess = InternetGetConnectedState( &DWFlags, 0 );
	if ( !BSuccess ) {
		   lua_pushboolean(L, FALSE);
	} else {
		   lua_pushboolean(L, TRUE);
	}

	return 1;
}

static struct luaL_Reg mobile[] = {
		{ "message", message },
		{ "isconnected", isconnected },
		{ NULL,NULL}
};


static int createTrayIcon() {
	return 0;
}


static int loadfileW(lua_State*L, wchar_t* fileName) {
	HANDLE h = CreateFileW(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL, NULL );
	int status = LUA_ERRFILE;
	if(h!=INVALID_HANDLE_VALUE) {
		DWORD fileSize = GetFileSize(h, NULL);
		DWORD sizeRead = 0;
		char* script = (char*)malloc(fileSize+16);
		memset(script, 0, fileSize+16);

		if(ReadFile(h, script,fileSize, &sizeRead, NULL )==TRUE) {
			status = luaL_loadbuffer(L, script, sizeRead, fileName);
		}
		free(script);
		CloseHandle(h);
	}
	return status;
}

int luaopen_lpeg (lua_State *L);

int _jason_openlibs(lua_State* L) {
	luaopen_socket_core(L); 	/* Opening the Socket library */
	luaopen_lfs(L);				/* Opening the Lua Filesystem library */
	luaopen_rings(L);			/* Opening the Rings library */
	luaopen_md5_core(L);			/* Opening the MD5 library */
	luaopen_base64(L);			/* Opening the Base64 library */
	luaopen_des56(L);			/* Opening the DES56 library */
	/* Opening the LPeg library */
	lua_pushcclosure(L, luaopen_lpeg, 0);
	lua_pushstring(L, "lpeg");
	lua_call(L, 1, 0);
}


int isdigit(int c) {
	return _isctype (c, _DIGIT)	;
}



int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine,
		int nCmdShow) {

	window = GetDesktopWindow();

	lua_State*L = luaL_newstate();
	if (L == NULL) {
		MessageBoxW(window,L"Fatal Error" , L"Error while initializing LuaState", 0);
		return -1;
	}

	lua_atpanic(L, lua_panic);

	luaL_openlibs(L);

	_jason_openlibs(L); 	/* Open the additional built-in libraries */


	luaL_register(L, "mobile", mobile);

	int status = 0;

	status = luaL_loadfile(L, INIT_FILE);

	if (status == LUA_ERRFILE)
		status = loadfileW(L, WINIT_FILE);

	if(status == LUA_ERRFILE) {
		MessageBoxW(window, _T(lua_tostring(L, -1)), L"Error opening init file", 0);
		return -1;
	}

	if(status!=0) {
		const char *s = lua_tostring(L, -1);
		MessageBoxW(window, _T(s), L"Error while compiling lua file", 0);
		return -1;
	}

	char param[BUFSIZ];
	wordToMultibyte(lpCmdLine, param);
	lua_pushstring(L, param);

	status = lua_pcall(L, 1, 0, 0);

	if(status!=0) {
		MessageBoxW(window, _T(lua_tostring(L, -1)), L"Error while initializing LuaState", 0);
		return -1;
	}


	lua_close(L);

	return 0;
}
