
#include <lua.hpp>
#include <LuaAide.h>
#include <lodepng.h>
#include <memory>
#include <iostream>

// decode         encode
// ======         ======
// from file      to file
// from memory    to memory
// lodepng_decode lodepng_encode    stateful
// lodepng_inspect

// LodePNGColorMode (colortype, bitdepth, paletteninfo) (construction, destruction, copy)
// - getter
// - add/clear palette

// LodePNGInfo (ancillary chunks) (construction, destruction, copy)
// - text chunks
// - icc profile

// lodepng_convert(): convert colortype

// chunk processing
// lodepng_chunk_next Iteration

// compression

using namespace std;

namespace {

    const auto modulename="github.com/vorgestern/LuaLodePng";
    const auto mtimage_name="mtimage";
    const void*mtimage_pointer=nullptr; // identify metatable via lua_topointer()

    const auto deleter=[](unsigned char*X)
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

    bool isimage(lua_State*L, int index)
    {
        LuaStack Q(L);
        if (Q.typeat(index)!=LuaType::TUSERDATA) return false;
        if (!lua_getmetatable(L, index)) return false;
        const void*p=lua_topointer(L, -1);
        Q.drop(1);
        return p==mtimage_pointer;
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

    int myconstructor(LuaStack&Q, unsigned char*pixels, unsigned width, unsigned height, LodePNGColorType colortype)
    {
        // Create userdata of type myimage**
        auto P=reinterpret_cast<myimage**>(lua_newuserdatauv(Q, sizeof(myimage*), 0));
        Q<<LuaValue(LUA_REGISTRYINDEX)<<LuaField(modulename)<<LuaField(mtimage_name)<<luarot3;
        Q.drop(2);
        lua_setmetatable(Q, -2);
        *P=new myimage {{pixels,deleter}, width, height, colortype};
        return 1;
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
        // printf("lodepng_decode_file(%s) ==> %p,%ux%u\n", filename.c_str(), out, width, height);
        // printf("X=%p erzeugt.\n", X);
        // delete X;
        return myconstructor(Q, out, width, height, colortype);
    }

    int chunklist(lua_State*L)
    {
        LuaStack Q(L);
        Q.argcheck(1, LuaType::TSTRING, "filename");
        // unsigned lodepng_load_file(unsigned char**out, size_t*outsize, const char*filename);
        unsigned char*out=nullptr;
        size_t outsize=0;
        const auto filename=Q.tostring(1);
        if (const auto rc=lodepng_load_file(&out, &outsize, filename.c_str()); rc!=0)
        {
            char pad[100];
            snprintf(pad, sizeof pad, "load_file fails with rc=%d (data=%p, %lu)", rc, out, outsize);
            return Q<<pad>>luaerror;
        }
        // const unsigned char*lodepng_chunk_next_const(const unsigned char*chunk, const unsigned char*end);
        Q<<LuaTable();
        const auto end=out+outsize;
        unsigned chunknum=0;
        for (const unsigned char*chunk=out+8; chunk<end; chunk=lodepng_chunk_next_const(chunk, end))
        {
            const auto len=lodepng_chunk_length(chunk);
            char chunktype[5];
            lodepng_chunk_type(chunktype, chunk);
            // printf("chunk %s: %u bytes\n", chunktype, len);

            Q<<LuaTable()
                <<chunktype>>LuaField("type")
                <<(int)len>>LuaField("len")
                <<(int)(chunk-(out+8))>>LuaField("offset")

                <<LuaTable()
                    <<(lodepng_chunk_ancillary(chunk)?"ancillary":"critical")>>LuaField("essential")
                    <<(lodepng_chunk_private(chunk)?"private":"public")>>LuaField("visible")
                    <<(lodepng_chunk_safetocopy(chunk)?true:false)>>LuaField("copyable")
                    <<(lodepng_chunk_check_crc(chunk)==0?true:false)>>LuaField("crc_ok")
                    >>LuaField("props")
                >>LuaElement({-2,++chunknum});
        }

        free(out);
        return 1;
    }

} // anon

#ifndef LODEPNG_EXPORTS
#define LODEPNG_EXPORTS
#endif

extern "C" LODEPNG_EXPORTS int luaopen_lodepng(lua_State*L)
{
    LuaStack Q(L);

    // Create registry entry for lualodepng
    Q<<LuaValue(LUA_REGISTRYINDEX)<<newtable>>LuaField(modulename);
    // Create metatable for images.
    Q<<LuaValue(LUA_REGISTRYINDEX)<<LuaField(modulename)
            <<newtable
                <<"image">>LuaMetaMethod::name
                <<myfinaliser>>LuaMetaMethod::gc
                <<mytostring>>LuaMetaMethod::tostring
                ;
    mtimage_pointer=lua_topointer(L, -1);
    Q       >>LuaField(mtimage_name);

    // =================================================================
    
    Q   <<LuaTable()
        <<"0.1">>LuaField("version")
        <<"https://github.com/vorgestern/LuaLodePNG.git">>LuaField("url")
        <<LuaTable()
            <<LODEPNG_VERSION_STRING>>LuaField("version")
            >>LuaField("lodepng")
        <<readfile>>LuaField("readfile")
        <<chunklist>>LuaField("chunklist")
    ;
    return 1;
}
