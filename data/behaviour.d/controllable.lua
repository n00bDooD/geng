local xacc = 0.5
local yacc = 0.5

local cooldown = 0

function scene_update(obj, current_time)
	local horinp = input.get('horizontal') * xacc
	local vertinp = input.get('vertical') * yacc

	obj:apply_impulse(vector.new(horinp,
				   vertinp))
	cooldown = cooldown - 1

	local shootinp = input.get('shoot') 
	if shootinp ~= 0 and cooldown < 0 then
		cooldown = 20
		local coin = scene.spawn_prefab('coin', 3)
		local objpos = obj:pos()
 		local addang = vector.forangle(obj:angle() + (3.14/2))
		coin:set_pos(objpos + (addang * 40))

		local xvel = (math.random() - 0.5) * 30
		coin:apply_impulse(addang * 30)
	end
end

function collision_begin(obj) 
	io.write('The ball has impacted something\n')
end

function collision_preSolve(obj) 
	--io.write('The ball is in preSolve state\n')
end

function collision_postSolve(obj) 
	--io.write('The ball is in postSolve state\n')
end

function collision_separate(obj) 
	io.write('The ball has stopped touching something\n')
end

