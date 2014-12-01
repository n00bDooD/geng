function collision_begin(obj) 
	io.write('The object has impacted something\n')
end

function collision_preSolve(obj) 
	--io.write('The object is in preSolve state\n')
end

function collision_postSolve(obj) 
	--io.write('The object is in postSolve state\n')
end

function collision_separate(obj) 
	io.write('The object has stopped touching something\n')
end

