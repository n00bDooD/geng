local air_speed = 0.6
local xacc = 0.8
local jump_strength = 35
local feet_offset_x = args[1]
local feet_offset = args[2]

function is_grounded(obj)
	return physics.segment_first(obj:pos() + vector.new(feet_offset_x, feet_offset - 0.1),
		obj:pos() + vector.new(feet_offset_x, feet_offset - 8)) ~= nil
end

local next_step = 0
local cur_step = 0

local ducked = false

local max_hold_jump_time = 0.25
local left_ground = 0
local cur_tick = 0

local last_vertical = 0

function scene_update(obj, step)
	cur_tick = cur_tick + step

	local horinp = input.get('horizontal') * xacc
	local vertinp = input.get('vertical')
	
	local vertj = 0
	if last_vertical == 0 then
		if vertinp < 0 then
			vertj = -1
		elseif vertinp > 0 then
			vertj = 1
		end
	end

	obj:set_angle(0)
	obj:foreach_collider(function(o, c) c:set_friction(20) end)

	local total_force = vector.zero()

	if is_grounded(obj) then
		left_ground = 0
		total_force = total_force + vector.new(horinp, 0)
		if vertj > 0 then
			audio.play(2)
			obj:apply_impulse(vector.new(0, jump_strength))
			left_ground = cur_tick
		elseif vertj < 0 then
			if not ducked then 
				audio.play(1)
				obj:send_message('cfunctest', 'FooTheBarBro')
			end
			ducked = true
			-- Set duck
		else
			ducked = false
		end
		if horinp > 0.01 then
			obj:foreach_collider(function(o, c) c:set_friction(0) end)
			obj:send_message('animation', 'walk_right')
		elseif horinp < -0.01 then
			obj:foreach_collider(function(o, c) c:set_friction(0) end)
			obj:send_message('animation', 'walk_left')
		else
			obj:send_message('animation', 'idle')
		end
	else
		if cur_tick - left_ground < max_hold_jump_time then
			if vertinp < 0 then vertinp = 0 end
			total_force = total_force + vector.new(0, vertinp * air_speed)
		end
		total_force = total_force + vector.new(horinp * air_speed, 0)
		if horinp > 0.01 then
			obj:send_message('animation', 'jump_right')
		elseif horinp < -0.01 then
			obj:send_message('animation', 'jump_left')
		else
			obj:send_message('animation', 'jump')
		end
	end

	--obj:foreach_collision(function(c)  total_force = total_force + total_force:project(c:contact_normal(0):normalize()) end)

	obj:apply_impulse(total_force)
	last_vertical = vertinp
end

