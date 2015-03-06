local friction = 5
local elasticity = 0.25

local o = scene.newobject()
o:set_name('bomb')
o:enable_physics(0.05, 0);
local c = o:add_circle_collider(35, vector.new(35*0.5, -35*0.5))
c:set_friction(friction)
c:set_elasticity(elasticity)

o:add_behaviour('animation', {{name='flash', speed=8, frames={{sprite=67}, {sprite=68}}}}, 'flash')

return o
