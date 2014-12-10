local o = scene.newobject()
o:set_name('hero')
o:enable_physics(0.1, math.huge);
local c = o:add_box_collider(60, 90)
o:add_behaviour("character_anim", 1, 2, 3, 4, {5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16})
o:add_behaviour("platformer")
return o
