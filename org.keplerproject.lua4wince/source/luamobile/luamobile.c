/*
** $Id: luamobile.c,v 1.1 2009-05-05 14:55:23 jasonsantos Exp $
** LuaMobile - Simple library to allow use of windows mobile-exclusive features
** currently only works on Windows Mobile 5
** Copyright 2009, keplerproject.org
** Released under the MIT License
** written by Luís Eduardo Jason Santos
*/

#include <windows.h>

#include <lua.h>
#include <lauxlib.h>

#define LIBNAME			"LuaMobile"
#define COPYRIGHT		"Copyright (C) 2009 Kepler Project"
#define DESCRIPTION		"LuaMobile is a Lua library to allow use of windows mobile-exclusive features"
#define VERSION			"0.5"

static void set_info (lua_State *L) {
	lua_pushliteral (L, "_COPYRIGHT");
	lua_pushliteral (L, COPYRIGHT);
	lua_settable (L, -3);
	lua_pushliteral (L, "_DESCRIPTION");
	lua_pushliteral (L, DESCRIPTION);
	lua_settable (L, -3);
	lua_pushliteral (L, "_VERSION");
	lua_pushliteral (L, LIBNAME VERSION);
	lua_settable (L, -3);
}

// TODO: create an instance to return
HWND window = NULL;

#define getoption(n,x) 	lua_getfield(L, n, #x); \
						if (lua_type(L, -1) != LUA_TNIL ) {\
							if(lua_isnumber(L, -1)) x = (int)lua_tonumber(L, -1); \
							else x = lua_tostring(L, -1); \
						}

/* message
 * Shows a message as an alert on the display
 * usage: luamobile.message(text)
 * @param 	text 	Text to be displayed
 */

int initialize(lua_State*L) {
	window = luaL_optinteger(L, 1, NULL);
}

/* messageBox
 * Shows a message as an alert on the display
 * usage: luamobile.message(text)
 * @param 	text 	Text to be displayed
 */

int messageBox(lua_State*L) {
	luaL_checkany(L, 1);
	MessageBoxW(window, _T(lua_tostring(L, 1)), _T("Lua 5.1"), 0);
	return 0;
}

/* isConnected
 * Checks whether the devis is connected to the internet
 * @param 	none
 */

int isConnected(lua_State*L) {
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
		{ "message", messageBox },
		{ "isconnected", isConnected },
		{ "init", initialize },
		{ NULL,NULL}
};




int luaopen_luamobile (lua_State *L);
int luaopen_luamobile (lua_State *L) {
	luaL_register (L, "luamobile", mobile);
	set_info (L);
	return 1;
}
