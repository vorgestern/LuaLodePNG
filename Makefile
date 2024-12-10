
XFILES   := main
CPPFLAGS := -I/usr/include/lua5.4 -I LuaAide/include
CXXFLAGS := --std=c++20 -Wall -Werror
.PHONY: clean dir

all: dir lualodepng.so LuaAide/libLuaAide.a
clean:
	@rm -rf b/* lualodepng.so
	@make -C LuaAide clean
dir:
	@mkdir -p b

# ============================================================

LuaAide/libLuaAide.a:
	make -j -C LuaAide all

# ============================================================

lualodepng.so: b/main.o LuaAide/libLuaAide.a
	g++ -shared -fpic -o $@ $^

b/%.o: src/%.cpp LuaAide/include/LuaAide.h
	g++ -c -Wall -Werror -fpic -o $@ $< $(CPPFLAGS) $(CXXFLAGS)
