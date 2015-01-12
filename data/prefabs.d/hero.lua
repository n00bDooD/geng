local o = scene.newobject()
o:set_name('hero')
o:enable_physics(0.1, math.huge);
local c = o:add_circle_collider(30, vector.new(0, 18))
c:set_friction(0.01)
local c = o:add_circle_collider(30, vector.new(0, -15))
c:set_friction(0.01)
o:add_behaviour("character_anim", 1, 2, 3, 4, {5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16})
o:add_behaviour("platformer", -45)
return o
