local o = scene.newobject()
o:set_name('buddy')
--o:enable_physics(0.1, math.huge);
local c = o:add_box_collider(60, 90)
c:set_friction(1)
o:add_behaviour("character_anim", 17, 18, 19, 20, {21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32})
return o
