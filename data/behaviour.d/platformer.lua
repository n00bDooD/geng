local air_speed = 0.2
local xacc = 0.5
local jump_strength = 10

function is_grounded(obj)
	return physics.segment_first(obj:pos() + vector.new(0, -0.1), obj:pos() + vector.new(0, -0.2)) ~= nil
end

local next_step = 0
local cur_step = 0

function scene_update(obj, step)
	cur_step = cur_step + step
	if cur_step > next_step then
		local object_state = 'idle'
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
		--obj:set_flipv(math.random() > 0.9)
		--obj:set_fliph(math.random() > 0.8)
		obj:send_message('character_anim', object_state)
	end

	local horinp = input.get('horizontal') * xacc
	local vertinp = input.get('vertical')

	if is_grounded(obj) then
		obj:apply_impulse(vector.new(horinp, 0))
		if vertinp > 0 then
			obj:apply_impulse(vector.new(0, jump_strength))
		elseif vertinp < 0 then
			-- Set duck
		end
	else
		obj:apply_impulse(vector.new(horinp * air_speed, 0))
	end
end

