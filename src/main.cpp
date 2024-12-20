
#include <lua.hpp>
#include <LuaAide.h>
#include <lodepng.h>
#include <memory>
#include <iostream>
#include "lualodepng.h"

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

    // Create registry entry for metatypes.
    Q<<LuaValue(LUA_REGISTRYINDEX)<<newtable>>LuaField(modulename);

    Q<<mkimage>>0;
    Q<<mkcolortype>>1;
    auto colortypes=Q.index(-1);

    // =================================================================

    // Create module table to return on the stack.
    Q   <<LuaTable()
        <<"0.1">>LuaField("version")
        <<"https://github.com/vorgestern/LuaLodePNG.git">>LuaField("url")
        <<LuaTable()
            <<LODEPNG_VERSION_STRING>>LuaField("version")
            >>LuaField("lodepng")
        <<readfile>>LuaField("readfile")       // Setzt den Typ mtimage voraus.
        <<chunklist>>LuaField("chunklist")     // Setzt den Typ mtimage voraus.

        <<colortypes>>LuaField("colortype");

    return 1;
}
