
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
}

)
