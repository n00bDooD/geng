function get_with(o)
	return 'elasticity=' ..
	o:get_elasticity() ..
	', friction=' ..
	o:get_friction() ..
	', surface_velocity=' ..
	tostring(o:get_surfvel())
end

function collision_begin(obj) 
	a, b = obj:get_objects()
	sa, sb = obj:get_colliders()
	io.write('Object ' .. tostring(a) .. 
		' shape ' .. tostring(sa) ..
		' has collided with ' .. tostring(b) ..
		' shape ' .. tostring(sb)  .. '\n')
	--io.write('The object has impacted something with ' .. get_with(obj) .. '\n')
end

function collision_preSolve(obj) 
	--io.write('The object is in preSolve state\n')
end

function collision_postSolve(obj) 
	--io.write('The object is in postSolve state\n')
	--io.write('Impact with energy ' .. obj:get_ke() .. '\n')
end

function collision_separate(obj) 
	--io.write('The object has stopped touching something with ' .. get_with(obj) .. '\n')
end

