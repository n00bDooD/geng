local spawntime = 1

function timeout(obj, current_time)
	spawntime = spawntime - current_time

	if spawntime < 0 then
		obj:delete()
	end
end

