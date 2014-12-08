function collision_begin(obj)
	a, b = obj:get_objects()
	if b:name() == 'killme' then
		b:delete()
		if a:name() == 'bullet' then
			a:delete()
		end
	end
end

