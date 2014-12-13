local o = scene.newobject()
o:set_name('blockermad')
--o:enable_physics(0.1, math.huge);
local c = o:add_box_collider(51, 51)
c:set_friction(1)
o:set_sprite(50)
return o
