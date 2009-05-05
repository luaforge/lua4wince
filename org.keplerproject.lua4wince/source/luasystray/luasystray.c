/*
** $Id: luasystray.c,v 1.1 2009-05-05 14:55:23 jasonsantos Exp $
** LuaSysTray - Simple Library to attach your script to the System Tray
** currently only works on Windows-based systems
** Copyright 2009, keplerproject.org
** Released under the MIT License
** written by Luís Eduardo Jason Santos
*/

#include <windows.h>

#include <lua.h>
#include <lauxlib.h>

#define LIBNAME			"LuaSysTray"
#define COPYRIGHT		"Copyright (C) 2009 Kepler Project"
#define DESCRIPTION		"LuaSysTray is a Lua library to attach your script to the System Tray"
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

#define getoption(n,x) 	lua_getfield(L, n, #x); \
						if (lua_type(L, -1) != LUA_TNIL ) {\
							if(lua_isnumber(L, -1)) x = (int)lua_tonumber(L, -1); \
							else x = lua_tostring(L, -1); \
						}

/*
 * Systray Entry Description Table
 *   icon : icon resource or filename to load icon
 *
 */

/* createicon
 *
 * Creates an icon resource from its filename
 *
 * usage: luasystray.createicon(<icon description table>)
 *
 * @param 	options 	Systray Icon Description Table:
 *
 * Systray Icon Description Table
 *   resourceid	: resource identifier to load icon from
 *   filename 	: icon filename to load icon from
 *   bitmap 	: true if icon file is a bitmap
 *	 width		: desired width for icon
 *	 height		: desired height for icon
 */
int createIcon(lua_State*L) {
	luaL_checkany(L, 1);
	int type = lua_type(L, 1);

	HMODULE hinstance = 0;
	int imgtype = IMAGE_ICON;

	const char*filename = NULL;
	const char*resourceid = NULL;

	int width = 0;
	int height = 0;
	int bitmap = 0;

	int loadtype = LR_LOADFROMFILE;

	if(type==LUA_TSTRING) {
		filename = lua_tostring(L, 1);
	}

	if(type==LUA_TTABLE) {
		getoption(1, resourceid);
		getoption(1, filename);
		getoption(1, bitmap);
		getoption(1, width);
		getoption(1, height);
	}

	if(bitmap) {
		imgtype = IMAGE_BITMAP;
		width = 0;
		height = 0;
	}

	if(resourceid) {
		loadtype = LR_DEFAULTCOLOR | LR_DEFAULTSIZE;
		hinstance = GetModuleHandleW(NULL);
		filename = resourceid;
	}

	HANDLE img = LoadImageW(
		hinstance,
		_T(filename),
		imgtype,
		width, height,
		loadtype
	);

	if(img) {
		luaL_error(L, "Error loading image");
	}

	lua_settop(L, 0);
	lua_pushlightuserdata(L, img);

	return 1;
}

/* add
 * Registers an icon on SysTray, along with all other options
 * usage: luasystray.add(<icon description table>)
 * @param 	options 	Systray Icon Description Table:
 * @returns	entry 		Entry to be used in other functions
 */
int addEntry(lua_State*L) {
	luaL_error(L, "Not implemented");
	return 0;
}

/* change
 * Registers an icon on SysTray, along with all other options
 * usage: luasystray.add(<icon description table>)
 * @param 	options 	Systray Icon Description Table:
 * @returns	entry 		Entry to be used in other functions
 */
int changeEntry(lua_State*L) {
	luaL_error(L, "Not implemented");
	return 0;
}

/* remove
 * Registers an icon on SysTray, along with all other options
 * usage: luasystray.add(<icon description table>)
 * @param 	options 	Systray Icon Description Table:
 * @returns	entry 		Entry to be used in other functions
 */
int removeEntry(lua_State*L) {
	luaL_error(L, "Not implemented");
	return 0;
}

static const struct luaL_reg lstlib[] = {
		{"createicon", createIcon},
		{"add", addEntry},
		{"change", changeEntry},
		{"remove", removeEntry},
		{NULL, NULL}
};


int luaopen_luasystray (lua_State *L);
int luaopen_luasystray (lua_State *L) {
	luaL_register (L, "luasystray", lstlib);
	set_info (L);
	return 1;
}
