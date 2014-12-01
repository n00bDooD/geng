local o = scene.newobject();
o:set_sprite(1);
o:enable_physics(0.1, 1);
local v = o:add_circle_collider(35);
v:set_elasticity(0.5)
v:set_friction(12)
o:add_behaviour("controllable")
return o
