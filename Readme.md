
# Purpose
LuaLodePNG is a Lua binding to lodepng (https://github.com/lvandeve/lodepng)

# Examples

    local llp=require "lodepng"
    print("Use lualodepng version", llp.version)
    print("Use lodepng version", llp.lodepng.version)
    local image=llp.readfile("demo.png")
    print("demo.png:", image)

    print "type\toffset\tlen\tdetails"
    for _,chunk in ipairs(llp.chunklist("demo.png")) do
        local P=chunk.props
        local details=string.format("essential='%s', copyable=%s, crc_ok=%s, visible='%s'", P.essential, P.copyable, P.crc_ok, P.visible)
        print(chunk.type,chunk.offset,chunk.len,details)
    end

# Requirements
+ C++ 20
+ Lua 5.4
## Submodules
+ [Lodepng](https://github.com/lvandeve/lodepng)
+ [LuaAide](https://github.com/vorgestern/LuaAide)
+ [ulutest](https://github.com/vorgestern/ulutest)

# How to build: First ...

    git submodule init
    git submodule update

## then on Linux ...

    make

creates lodepng.so and executes unit test.
Copy the shared object to where Lua will find it with `require "lodepng"`.

## else on Windows ...

Edit buildsys/VS17/Lua.props to point to your installation of Lua.
Build with Visual Studio 2022 (VS17).<br/>
Execute unit test with

    lua unittest.lua

The Release build (32 bit) will create lodepng.dll in the repository root directory,
the Debug build in buildsys/VS17/Debug. Copy the DLL to where Lua will find it with
`require "lodepng"`.

# Status
Version 0.1 Preliminary - under development

    readfile(filename)      reads a png-file, returns image object
    chunklist(filename)     returns a list of items, that contain information about the chunks in the file.
                            Example:
                            IHDR    13      0       {essential='critical', copyable=false, crc_ok=true, visible='public'}
                            gAMA    4       25      {essential='ancillary', copyable=false, crc_ok=true, visible='public'}
                            IDAT    4255    41      {essential='critical', copyable=false, crc_ok=true, visible='public'}
                            IEND    0       4308    {essential='critical', copyable=false, crc_ok=true, visible='public'}
