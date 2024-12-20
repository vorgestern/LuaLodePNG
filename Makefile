
XFILES   := main
CPPFLAGS := -I/usr/include/lua5.4 -ILuaAide/include -Ilodepng
CXXFLAGS := --std=c++20 -Wall -Werror
.PHONY: clean dir test

all: dir lodepng.so LuaAide/libLuaAide.a ulutest/ulutest.so test
clean:
	@rm -rf b/* lodepng.so
	@make -C LuaAide clean
	@make -C ulutest clean
dir:
	@mkdir -p b

# ============================================================

LuaAide/libLuaAide.a:
	make -j -C LuaAide all

# ============================================================

ulutest/ulutest.so:
	make -j -C ulutest all

# ============================================================

lodepng.so: b/main.o b/image.o b/colortype.o b/lodepng.o LuaAide/libLuaAide.a
	g++ -shared -fpic -o $@ $^

b/lodepng.o: lodepng/lodepng.cpp lodepng/lodepng.h
	g++ -c -Wall -Werror -fpic -o $@ $< $(CPPFLAGS) $(CXXFLAGS)

b/%.o: src/%.cpp src/lualodepng.h LuaAide/include/LuaAide.h
	g++ -c -Wall -Werror -fpic -o $@ $< $(CPPFLAGS) $(CXXFLAGS)

# ============================================================

test: lodepng.so
	lua unittest.lua

# More enums
# LodePNGFilterStrategy

# Structs
# LodePNGColorMode
# LodePNGTime
# LodePNGInfo
# LodePNGDecoderSettings
# LodePNGColorStats
# LodePNGEncoderSettings
# LodePNGState
