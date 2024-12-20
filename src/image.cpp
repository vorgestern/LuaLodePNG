
#include <lua.hpp>
#include <LuaAide.h>
#include <lodepng.h>
#include <memory>
#include <iostream>
#include "lualodepng.h"

using namespace std;

namespace {

    mtinfo mtimage={"mtimage", nullptr};

    const auto deleter=[](unsigned char*X)
    {
        printf("delete pixels %p\n", X);
        free(X);
    };

    using mypixels=unique_ptr<unsigned char,void(*)(unsigned char*)>;

    struct myimage
    {
        mypixels pixels;
        unsigned width, height;
        LodePNGColorType colortype;
    };

    bool isimage(lua_State*L, int index)
    {
        LuaStack Q(L);
        if (Q.typeat(index)!=LuaType::TUSERDATA) return false;
        if (!lua_getmetatable(L, index)) return false;
        const void*p=lua_topointer(L, -1);
        Q.drop(1);
        return p==mtimage.ident;
    }

    int mytostring(lua_State*L)
    {
        LuaStack Q(L);
        Q.argcheck(1, isimage, "image");
        const myimage*I=*reinterpret_cast<myimage**>(lua_touserdata(Q, 1));
        // cout<<"Hier ist mytostring "<<Q<<"\n";
        // printf("myimage pointer=%p\n", I);
        char pad[100];
        const size_t nw=snprintf(pad, sizeof(pad), "%u x %u", I->width, I->height);
        return Q<<string_view {pad, nw}, 1;
    }

    int myfinaliser(lua_State*L)
    {
        // printf("myfinaliser\n");
        LuaStack Q(L);
        if (Q.hasheavyuserdataat(-1))
        {
            auto X=Q.touserpointer<myimage**>(-1);
            // printf("finaliser deletes %p\n", X);
            *X=nullptr;
            delete*X;
        }
        return 0;
    }

} // anon

int myconstructor(LuaStack&Q, unsigned char*pixels, unsigned width, unsigned height, LodePNGColorType colortype)
{
    // Create userdata of type myimage**
    auto P=reinterpret_cast<myimage**>(lua_newuserdatauv(Q, sizeof(myimage*), 0));
    Q<<LuaValue(LUA_REGISTRYINDEX)<<LuaField(modulename)<<LuaField(mtimage.name)<<luarot3;
    Q.drop(2);
    lua_setmetatable(Q, -2);
    *P=new myimage {{pixels,deleter}, width, height, colortype};
    return 1;
}

int mkimage(lua_State*L)
{
    LuaStack Q(L);
    Q   <<LuaValue(LUA_REGISTRYINDEX)<<LuaField(modulename)
            <<newtable
                <<"image">>LuaMetaMethod::name
                <<myfinaliser>>LuaMetaMethod::gc
                <<mytostring>>LuaMetaMethod::tostring
    ;
    mtimage.ident=lua_topointer(L, -1);
    Q.dup()>>LuaMetaMethod::index;
    Q>>LuaField(mtimage.name);
    return 0;
}
