local duck 	= args[1]
local front 	= args[2]
local hurt 	= args[3]
local jump 	= args[4]

local walk_cycle = args[5]
local walk_speed = 12

local object_state = 'walk'
local walk_idx = 1

local next_step = 0
local cur_step = 0
function scene_update(obj, step)
	cur_step = cur_step + step
	if cur_step > next_step then
		next_step = math.ceil(cur_step)
		local r = math.random()
		if r > 0.3 then 
			object_state = 'walk'
		elseif r > 0.3 then
			object_state = 'hurt'
		elseif r > 0.2 then
			object_state = 'duck'
		elseif r > 0.1 then
			object_state = 'jump'
		else
			object_state = 'idle'
		end
	end

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
