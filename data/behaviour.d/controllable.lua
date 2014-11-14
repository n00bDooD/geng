xacc = 2
yacc = 0.5

function update(obj, current_time)
	local horinp = input.get_axis('horizontal')
	local vertinp = input.get_axis('vertical')

	obj:apply_force(vector.new(horinp * xacc,
				   vertinp * yacc))
end

