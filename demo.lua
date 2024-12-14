
local X=require "lodepng"

for k,v in pairs(X) do print(k,v) end
for k,v in pairs(X.lodepng) do print("lodepng", k, v) end

local function demo(filename)
    local a=X.readfile(filename)
    print("image:", a)
end

demo "demo.png"
