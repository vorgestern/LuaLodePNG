
local X=require "lualodepng"

for k,v in pairs(X) do print(k,v) end
for k,v in pairs(X.lodepng) do print("lodepng", k, v) end

local a=X.readfile "demo.png"
