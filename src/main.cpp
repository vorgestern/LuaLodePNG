
#include <lua.hpp>
#include <LuaAide.h>

using namespace std;

#ifndef LUALODEPNG_EXPORTS
#define LUALODEPNG_EXPORTS
#endif

extern "C" LUALODEPNG_EXPORTS int luaopen_lualodepng(lua_State*L)
{
    LuaStack Q(L);
    Q   <<LuaTable()
        <<"0.1">>LuaField("version")
        <<"https://github.com/vorgestern/LuaLodePng.git">>LuaField("url");
    return 1;
}
