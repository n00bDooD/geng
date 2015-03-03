local spawntime = args[1]

function update(obj, current_time)
	spawntime = spawntime - current_time

	if spawntime < 0 then
		obj:delete()
	end
end

