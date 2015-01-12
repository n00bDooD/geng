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

local ducked = false

local max_hold_jump_time = 0.25
local left_ground = 0
local cur_tick = 0

function scene_update(obj, step)
	cur_tick = cur_tick + step

	local horinp = input.get('horizontal') * xacc
	local vertinp = input.get('vertical')

	obj:set_angle(0)

	if is_grounded(obj) then
		left_ground = 0
		obj:apply_impulse(vector.new(horinp, 0))
		if vertinp > 0 then
			audio.play(2)
			obj:apply_impulse(vector.new(0, jump_strength))
			left_ground = cur_tick
		elseif vertinp < 0 then
			if not ducked then audio.play(1) end
			ducked = true
			-- Set duck
		else
			ducked = false
		end
		if horinp > 0.01 then
			obj:send_message('character_anim', {state='walk',direction='right'})
		elseif horinp < -0.01 then
			obj:send_message('character_anim', {state='walk',direction='left'})
		else
			obj:send_message('character_anim', 'idle')
		end
	else
		if cur_tick - left_ground < max_hold_jump_time then
			if vertinp < 0 then vertinp = 0 end
			obj:apply_impulse(vector.new(0, vertinp * air_speed))
		end
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

