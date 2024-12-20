
#include <lua.hpp>
#include <LuaAide.h>
#include <lodepng.h>
#include <memory>
#include <iostream>
#include "lualodepng.h"

using namespace std;

//  LCT_GREY = 0, // grayscale: 1,2,4,8,16 bit
//  LCT_RGB = 2, // RGB: 8,16 bit
//  LCT_PALETTE = 3, // palette: 1,2,4,8 bit
//  LCT_GREY_ALPHA = 4, // grayscale with alpha: 8,16 bit
//  LCT_RGBA = 6, // RGB with alpha: 8,16 bit
//  /* LCT_MAX_OCTET_VALUE lets the compiler allow this enum to represent any invalid
//  byte value from 0 to 255 that could be present in an invalid PNG file header. Do
//  not use, compare with or set the name LCT_MAX_OCTET_VALUE, instead either use
//  the valid color type names above, or numeric values like 1 or 7 when checking for
//  particular disallowed color type byte values, or cast to integer to print it. */
//  LCT_MAX_OCTET_VALUE = 255

namespace {

    mtinfo mtcolortype {"mtcolortype", nullptr};

    int neu(lua_State*L)
    {
        LuaStack Q(L);
        LodePNGColorType*neu=reinterpret_cast<LodePNGColorType*>(lua_newuserdatauv(Q, sizeof(LodePNGColorType), 0));
        Q<<luaswap; lua_setmetatable(Q, -2);
        *neu=static_cast<LodePNGColorType>(Q.toint(1));
        return 1;
    }

    bool iscolortype(lua_State*L, int index)
    {
        LuaStack Q(L);
        if (!lua_getmetatable(L, index)) return false;
        const void*p=lua_topointer(L, -1);
        Q.drop(1);
        return p==mtcolortype.ident;
    }

    int mytostring(lua_State*L)
    {
        LuaStack Q(L);
        Q.argcheck(1, iscolortype, "colortype");
        const auto j=*reinterpret_cast<LodePNGColorType*>(lua_touserdata(Q, 1));
        switch (j)
        {
            case LCT_GREY: return Q<<"LCT_GREY", 1;
            case LCT_RGB: return Q<<"LCT_RGB", 1;
            case LCT_PALETTE: return Q<<"LCT_PALETTE", 1;
            case LCT_GREY_ALPHA: return Q<<"LCT_GREY_ALPHA", 1;
            case LCT_RGBA: return Q<<"LCT_RGBA", 1;
            default: return Q<<"unknown", 1;
        }
    }

    int mynumericvalue(lua_State*L)
    {
        LuaStack Q(L);
        Q.argcheck(1, iscolortype, "colortype");
        const auto j=*reinterpret_cast<LodePNGColorType*>(lua_touserdata(Q, 1));
        return Q<<static_cast<int>(j), 1;
    }

}

int mkcolortype(lua_State*L)
{
    LuaStack Q(L);
    Q   <<LuaValue(LUA_REGISTRYINDEX)<<LuaField(modulename)
            <<newtable
                <<"colortype">>LuaMetaMethod::name
                <<mytostring>>LuaMetaMethod::tostring
                <<mynumericvalue>>LuaField("numeric")
    ;
    mtcolortype.ident=lua_topointer(L, -1);
    Q.dup()>>LuaMetaMethod::index;
    Q>>LuaField(mtcolortype.name);

    // Keep metatable on stack.
    Q<<LuaValue(LUA_REGISTRYINDEX)<<LuaField(modulename)<<LuaField(mtcolortype.name)<<luarot3;
    Q.drop(2);
    auto mt=Q.index(-1);

    Q<<LuaTable();
    Q<<neu<<LCT_GREY<<mt>>1;        Q>>LuaField("LCT_GREY");
    Q<<neu<<LCT_RGB<<mt>>1;         Q>>LuaField("LCT_RGB");
    Q<<neu<<LCT_PALETTE<<mt>>1;     Q>>LuaField("LCT_PALETTE");
    Q<<neu<<LCT_GREY_ALPHA<<mt>>1;  Q>>LuaField("LCT_GREY_ALPHA");
    Q<<neu<<LCT_RGBA<<mt>>1;        Q>>LuaField("LCT_RGBA");
    return 1;
}
