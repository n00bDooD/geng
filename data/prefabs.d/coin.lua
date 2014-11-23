local o = scene.newobject();
o:set_sprite(5);
o:enable_physics(0.1,1);
local c = o:add_circle_collider(20)
c:set_elasticity(0.01)
c:set_friction(1.2)
o:add_behaviour('timeout', prefab_args[1])
return o
