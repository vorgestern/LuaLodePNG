
#include <lua.hpp>
#include <LuaAide.h>
#include <lodepng.h>
#include <memory>

using namespace std;

namespace {

    static const auto deleter=[](unsigned char*X)
    {
        printf("delete pixels %p\n", X);
        free(X);
    };
    using mypixels=std::unique_ptr<unsigned char,void(*)(unsigned char*)>;

    struct myimage
    {
        mypixels pixels;
        unsigned width, height;
        LodePNGColorType colortype;
    };

    int readfile(lua_State*L)
    {
        LuaStack Q(L);
        Q.argcheck(1, LuaType::TSTRING, "filename");

        unsigned char*out=nullptr;
        unsigned width=0, height=0;
        const auto filename=Q.tostring(1);
        const auto colortype=LCT_RGB;
        if (const auto rc=lodepng_decode_file(&out, &width, &height, filename.c_str(), colortype, 8); rc!=0)
        {
            char pad[100];
            snprintf(pad, sizeof pad, "decode_file fails with rc=%d", rc);
            return Q<<pad>>luaerror;
        }
        printf("lodepng_decode_file(%s) ==> %p,%ux%u\n", filename.c_str(), out, width, height);
        auto*X=new myimage {{out,deleter}, width, height, colortype};
        printf("X=%p erzeugt.\n", X);
        delete X;
        return 0;
    }

} // anon

#ifndef LUALODEPNG_EXPORTS
#define LUALODEPNG_EXPORTS
#endif

extern "C" LUALODEPNG_EXPORTS int luaopen_lualodepng(lua_State*L)
{
    LuaStack Q(L);
    Q   <<LuaTable()
        <<"0.1">>LuaField("version")
        <<"https://github.com/vorgestern/LuaLodePng.git">>LuaField("url")
        <<LuaTable()
            <<LODEPNG_VERSION_STRING>>LuaField("version")
            >>LuaField("lodepng")
        <<readfile>>LuaField("readfile");
    ;
    return 1;
}
