local duck 	= args[1]
local front 	= args[2]
local hurt 	= args[3]
local jump 	= args[4]

local walk_cycle = args[5]
local walk_speed = 12

local object_state = 'idle'
local walk_idx = 1
function receive(message) if type(message) == 'string' then object_state = message; io.write('New state ' .. message .. '\n') end end

function scene_update(obj, step)
	if object_state == 'idle' then
		obj:set_sprite(front)
	elseif object_state == 'duck' then
		obj:set_sprite(duck)
	elseif object_state == 'jump' then
		obj:set_sprite(jump)
	elseif object_state == 'hurt' then
		obj:set_sprite(hurt)
	elseif object_state == 'walk' then
		walk_idx = walk_idx + step * walk_speed
		if walk_idx >= #walk_cycle then
			local remainder = walk_idx - #walk_cycle
			walk_idx = 1 + remainder
		end
		local s = walk_cycle[math.ceil(walk_idx)]
		if s ~= nil then obj:set_sprite(s) end
	end
end
