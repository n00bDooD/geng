local o = scene.newobject();
local c = o:add_circle_collider(35);
c:set_friction(0.74)
o:set_sprite(3);
return o
