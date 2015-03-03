local o = scene.newobject()
o:set_name('blockermad')
local c = o:add_box_collider(51, 51)
c:set_friction(0.1)
o:set_sprite(50)
return o
