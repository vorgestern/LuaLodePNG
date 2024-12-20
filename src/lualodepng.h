
const std::string_view modulename="github.com/vorgestern/LuaLodePng";

struct mtinfo
{
    std::string_view name;
    const void*ident{nullptr};
};

int mkimage(lua_State*);
int myconstructor(LuaStack&, unsigned char*pixels, unsigned width, unsigned height, LodePNGColorType colortype);

int mkcolortype(lua_State*);
