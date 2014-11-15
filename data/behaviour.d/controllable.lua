local xacc = 2
local yacc = 1

function update(obj, current_time)
	local horinp = input.get('horizontal')
	local vertinp = input.get('vertical')

	io.write(string.format("Force (%f,%f)", horinp, vertinp) .. '\n')

	obj:apply_force(vector.new(horinp,
				   vertinp))
end

coroutine.yield()

