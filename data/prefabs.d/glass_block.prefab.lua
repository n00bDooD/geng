local o = scene.newobject();
local c =o:add_box_collider(220, 70)
c:set_friction(0.9)
o:set_sprite(4);
return o
