local o = scene.newobject();
local c = o:add_box_collider(220, 70)
c:set_friction(0.1)
c:set_elasticity(1)
o:set_sprite(4);
return o
