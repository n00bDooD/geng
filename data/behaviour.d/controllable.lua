local xacc = 2
local yacc = 2

local cooldown = 0

function controllable(obj, current_time)
	local horinp = input.get('horizontal') * xacc
	local vertinp = input.get('vertical') * yacc

	obj:apply_impulse(vector.new(horinp,
				   vertinp))
	cooldown = cooldown - 1

	local shootinp = input.get('shoot') 
	if shootinp ~= 0 and cooldown < 0 then
		cooldown = 20
		local coin = scene.spawn_prefab('coin')
		local objpos = obj:pos()
		objpos:sety(objpos:y() + 35 + 40)
		coin:set_pos(objpos)

		local xvel = (math.random() - 0.5) * 30
		coin:apply_impulse(vector.new(xvel, 15))
	end
end

coroutine.yield()

