
const std::string_view modulename="github.com/vorgestern/LuaLodePng";

struct mtinfo
{
    std::string_view name;
    const void*ident{nullptr};
};

int mkcolortype(lua_State*);
