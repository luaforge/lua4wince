#define LUA_CORE

#define luaall_c

#ifdef CONSOLE
#include"lua/lua.c"
#endif

#include "lua/luaconf.h"

#include "lua/lmem.c"
#include "lua/lobject.c"
#include "lua/lopcodes.c"
#include "lua/lparser.c"
#include "lua/lstate.c"
#include "lua/ltable.c"
#include "lua/ltm.c"
#include "lua/lundump.c"
#include "lua/lvm.c"
#include "lua/lzio.c"

#include "lua/lapi.c"
#include "lua/lcode.c"
#include "lua/ldebug.c"
#include "lua/ldo.c"
#include "lua/ldump.c"
#include "lua/lfunc.c"
#include "lua/lgc.c"
#include "lua/llex.c"


#include "lua/lauxlib.c"
#include "lua/lbaselib.c"
#include "lua/ldblib.c"
#include "lua/linit.c"
#include "lua/liolib.c"
#include "lua/lmathlib.c"
#include "lua/loadlib.c"
#include "lua/loslib.c"
#include "lua/lstring.c"
#include "lua/lstrlib.c"
#include "lua/ltablib.c"
