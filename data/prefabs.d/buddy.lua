local o = scene.newobject()
o:set_name('buddy')
--o:enable_physics(0.1, math.huge);
local c = o:add_circle_collider(30, vector.new(0, 18))
c:set_friction(1)
local c = o:add_circle_collider(30, vector.new(0, -15))
c:set_friction(1)
return o
