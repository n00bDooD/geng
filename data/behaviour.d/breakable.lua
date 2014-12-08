local damage_sprites = args[1]
local hp = args[2]
local max_hp = hp

function scene_update(obj)
	local i = #damage_sprites - math.floor((hp / max_hp) * #damage_sprites)
	obj:set_sprite(damage_sprites[math.max(1, math.min(#damage_sprites, i))])

	if hp < 0 then
		obj:delete()
	end
end

function collision_postSolve(collision)
	if collision:get_ke() > 1000 then
		hp = hp - 1
	end
end
