
-- Prepare Lua's search path so ulutest will be found.
local bpattern={
    ["/"]="ulutest/?.so;",
    ["\\"]="ulutest\\?.dll;",
}
package.cpath=(bpattern[package.config:sub(1,1)] or "")..package.cpath

local ok,X=pcall(require, "lodepng")

if not ok then
    error("\n\tThis is a test suite for 'lodepng'."..
    "\n\tHowever, require 'lodepng' failed."..
    "\n\tBuild it right here.")
end

local ok,ULU=pcall(require, "ulutest")

if not ok then
    error("\n\tThis is a Unit Test implemented with 'ulutest'."..
    "\n\tHowever, require 'ulutest' failed."..
    "\n\tBuild it as a submodule right here.")
end

local TT=ULU.TT

ULU.RUN(

{
    name="version",
    TT("present", function(T) T:ASSERT_EQ("string", type(X.version)) end),
    TT("value", function(T) T:ASSERT_EQ("0.1", X.version) end)
},

{
    name="url",
    TT("present", function(T) T:ASSERT_EQ("string", type(X.url)) end),
    TT("value", function(T) T:ASSERT_EQ("https://github.com/vorgestern/LuaLodePNG.git", X.url) end)
},

{
    name="lodepng.version",
    TT("present", function(T) T:ASSERT_EQ("string", type(X.lodepng.version)) end),
    TT("value", function(T) T:ASSERT_EQ("20241015", X.lodepng.version) end)
},

{
    name="colortype",
    TT("present", function(T)
        T:ASSERT_EQ("table", type(X.colortype))
    end),
    TT("values", function(T)
        T:ASSERT(X.colortype.LCT_GREY)
        T:ASSERT(X.colortype.LCT_RGB)
        T:ASSERT(X.colortype.LCT_PALETTE)
        T:ASSERT(X.colortype.LCT_GREY_ALPHA)
        T:ASSERT(X.colortype.LCT_RGBA)
    end),
    TT("tostring", function(T)
        T:ASSERT_EQ("LCT_GREY", tostring(X.colortype.LCT_GREY))
        T:ASSERT_EQ("LCT_RGB", tostring(X.colortype.LCT_RGB))
        T:ASSERT_EQ("LCT_PALETTE", tostring(X.colortype.LCT_PALETTE))
        T:ASSERT_EQ("LCT_GREY_ALPHA", tostring(X.colortype.LCT_GREY_ALPHA))
        T:ASSERT_EQ("LCT_RGBA", tostring(X.colortype.LCT_RGBA))
    end),
    TT("numeric", function(T)
        T:ASSERT_EQ(0, X.colortype.LCT_GREY:numeric())
        T:ASSERT_EQ(2, X.colortype.LCT_RGB:numeric())
        T:ASSERT_EQ(3, X.colortype.LCT_PALETTE:numeric())
        T:ASSERT_EQ(4, X.colortype.LCT_GREY_ALPHA:numeric())
        T:ASSERT_EQ(6, X.colortype.LCT_RGBA:numeric())
    end)
}

)
