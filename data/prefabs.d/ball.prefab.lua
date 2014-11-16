local o = scene.newobject();
o:set_sprite(1);
local v = o:add_circle_collider(35);
v:set_elasticity(0.25)
v:set_friction(0.7)
o:enable_physics(1, 1);
o:add_behaviour("controllable")
return o
