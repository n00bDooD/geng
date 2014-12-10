local air_speed = 0.6
local xacc = 0.8
local jump_strength = 35
local feet_offset = args[1]

function is_grounded(obj)
	return physics.segment_first(obj:pos() + vector.new(0, feet_offset - 0.1),
		obj:pos() + vector.new(0, feet_offset - 0.2)) ~= nil
end

local next_step = 0
local cur_step = 0

function scene_update(obj, step)

	local horinp = input.get('horizontal') * xacc
	local vertinp = input.get('vertical')

	if is_grounded(obj) then
		obj:apply_impulse(vector.new(horinp, 0))
		if vertinp > 0 then
			obj:apply_impulse(vector.new(0, jump_strength))
		elseif vertinp < 0 then
			-- Set duck
		end
		if horinp > 0.01 then
			obj:send_message('character_anim', {state='walk',direction='right'})
		elseif horinp < -0.01 then
			obj:send_message('character_anim', {state='walk',direction='left'})
		else
			obj:send_message('character_anim', 'idle')
		end
	else
		obj:apply_impulse(vector.new(horinp * air_speed, 0))
		if horinp > 0.01 then
			obj:send_message('character_anim', {state='jump',direction='right'})
		elseif horinp < -0.01 then
			obj:send_message('character_anim', {state='jump',direction='left'})
		else
			obj:send_message('character_anim', 'jump')
		end
	end
end

