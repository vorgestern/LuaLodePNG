
local X=require "lodepng"

for k,v in pairs(X) do print(k,v) end
for k,v in pairs(X.lodepng) do print("lodepng", k, v) end

local function demo(filename)
    local a=X.readfile(filename)
    print("image:", a)
end

local function demo1(filename)
    local function propstr(P)
        return string.format("{essential='%s', copyable=%s, crc_ok=%s, visible='%s'}", P.essential, P.copyable, P.crc_ok, P.visible)
    end

    local Chunks=X.chunklist(filename)
    print(string.format("\nFound %d chunks.", #Chunks))
    print "index\ttype\tlen\toffset\tproperties"
    for j,c in ipairs(Chunks) do print(j,c.type,c.len,c.offset,propstr(c.props)) end
end

demo1 "demo.png"
