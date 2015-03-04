function add_sprites(thandle, sprites)
	for k, s in ipairs(sprites) do
		local spritehandle = renderer.add_sprite(thandle, s.w * 0.5, s.h* 0.5, s.x, s.y, s.w, s.h)
		if s.name ~= nil then
			io.write(spritehandle .. ':\t' .. s.name .. '\n')
		end
	end
end

local func = loadfile('data/textures/sheets/p1.lua'); func()
local func = loadfile('data/textures/sheets/p2.lua'); func()
local func = loadfile('data/textures/sheets/p3.lua'); func()

local func = loadfile('data/textures/sheets/enemies.lua'); func()
local func = loadfile('data/textures/sheets/items.lua'); func()
local func = loadfile('data/textures/sheets/tiles.lua'); func()
local func = loadfile('data/textures/sheets/hud.lua'); func()

renderer.set_background(renderer.add_texture('data/textures/bg_castle.tga'))
