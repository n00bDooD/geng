local o = scene.newobject();
local c = o:add_box_collider(140, 70);
c:set_friction(0.2)
o:set_sprite(2);
return o
