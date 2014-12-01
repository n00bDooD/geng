local o = scene.newobject();
local c = o:add_box_collider(140, 70);
c:set_friction(0.7)
c:set_elasticity(0.05)
o:set_sprite(2);
return o
