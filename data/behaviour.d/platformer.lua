local feet_offset_x = args[1]
local feet_offset = args[2]

local xspeed = 20
local deceleration = 50
local max_velocity = vector.new(7,20)

local air_speed = 0.6
local jump_strength = 600

function is_grounded(obj)
	return physics.segment_first(obj:pos() + vector.new(feet_offset_x, feet_offset - 0.1),
		obj:pos() + vector.new(feet_offset_x, feet_offset - 8)) ~= nil
end

local velocity = vector.zero()
local step = 0
local cur_tick = 0

local accel = vector.zero()
function apply(force, x)
	if x == nil then
		--io.write(cur_tick .. ' Acceleration of: ' .. tostring(force) .. '\n')
		--velocity = velocity + (force * step)
		accel = accel + (force)
	else
		apply(vector.new(force, x))
	end
end

local max_hold_jump_time = 0.25
local jumped = false
local left_ground = 0

local last_vertical = 0

function update(obj, s)
	accel = vector.zero()
	step = s
	cur_tick = cur_tick + s

	local horinp = input.get('horizontal') * xspeed

	if velocity:x() < 1 and velocity:x() > -1 then
		horinp = horinp * 10
	end

	local vertinp = input.get('vertical')
	local vertedge = 0
	if last_vertical == 0 then
		if vertinp < 0 then
			vertedge = -1
		elseif vertinp > 0 then
			vertedge = 1
		end
	end

	if is_grounded(obj) then
		if velocity:y() < 0 then
			velocity:sety(0)
			accel:sety(0)
		end
		jumped = false
		left_ground = 0

		apply(horinp, 0)

		if horinp < 0 then
			if velocity:x() > 0 then
				apply(-deceleration * velocity:x(), 0)
			end
		elseif horinp > 0 then
			if velocity:x() < 0 then
				apply(deceleration * -velocity:x(), 0)
			end
		else
			if velocity:x() > 0 then
				apply(-deceleration * velocity:x(), 0)
			else
				apply(deceleration * -velocity:x(), 0)
			end
		end

		-- jumping
		if vertedge > 0 then
			audio.play(2)
			left_ground = cur_tick
			jumped = true
			apply(0, jump_strength)
		end
	else
		--io.write('grav ')
		apply(physics.gravity())
		apply(horinp * air_speed, 0)

		if jumped and vertinp <= 0 then
			-- Apply extra gravity if the player let go
			-- of the jump button
			apply(physics.gravity())
		end
		if cur_tick - left_ground < max_hold_jump_time and vertinp > 0 then
			-- Apply extra speed upwards when the player is
			-- holding the jump button
			apply(0, vertinp * air_speed)
		end

		-- inform other parts about our state
		if horinp > 0.01 then
			obj:send_message('animation', 'jump_right')
		elseif horinp < -0.01 then
			obj:send_message('animation', 'jump_left')
		else
			obj:send_message('animation', 'jump')
		end

	end

	velocity = velocity + (accel * step)
	if velocity:x() < 0.01 and velocity:x() > -0.01 then
		velocity:setx(0)
	end
	if velocity:y() < 0.01 and velocity:y() > -0.01 then
		velocity:sety(0)
	end

	if velocity:x() > 0 then
		if velocity:x() > max_velocity:x() then
			velocity:setx(max_velocity:x())
		end
	else
		if velocity:x() < -max_velocity:x() then
			velocity:setx(-max_velocity:x())
		end
	end
	if velocity:y() > 0 then
		if velocity:y() > max_velocity:y() then
			velocity:sety(max_velocity:y())
		end
	else
		if velocity:y() < -max_velocity:y() then
			velocity:sety(-max_velocity:y())
		end
	end
	--io.write(tostring(velocity)..'\n')
	obj:set_pos(obj:pos() + (velocity))
	last_vertical = vertinp
end 
