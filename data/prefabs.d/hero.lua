local o = scene.newobject()
o:set_name('hero')
o:enable_physics(0.1, 0);
local friction = 10
local elasticity = 0
local c = o:add_circle_collider(30, vector.new(15, 18))
c:set_friction(friction)
c:set_elasticity(elasticity)
local c = o:add_circle_collider(30, vector.new(15, -15))
c:set_friction(friction)
c:set_elasticity(elasticity)

function new_anim(name, sprites)
	local a = {}
	a.name = name
	a.frames = {}
	a.speed = 1
	for i = 1, #sprites do
		if type(sprites[i]) == 'number' then
			a.frames[i] = {sprite=sprites[i], speed=1};
		else
			a.frames[i] = sprites[i];
		end
	end
	return a
end

local anim = {}

anim.duck  = new_anim('duck' , {1})
anim.idle = new_anim('idle', {2})
anim.hurt  = new_anim('hurt' , {3})

anim.jump_left  = {name='jump_left', frames={{sprite=4, fliph=true, speed=1}}, speed=1}
anim.jump_right  = {name='jump_right', frames={{sprite=4, fliph=false, speed=1}}, speed=1}
anim.jump = {name='jump', frames={{sprite=4, speed=1}}, speed=1}

anim.walk_left = new_anim('walk_left', {5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16})
anim.walk_right = new_anim('walk_right', {5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16})

anim.walk_left.speed = #anim.walk_left.frames * 2
anim.walk_right.speed = #anim.walk_right.frames * 2

for i = 1, #anim.walk_left.frames do
	anim.walk_left.frames[i].fliph = true
end
for i = 1, #anim.walk_right.frames do
	anim.walk_right.frames[i].fliph = false
end

o:add_behaviour('animation', anim, 'idle')

o:add_behaviour("platformer", 15, -45)
o:set_ang_vel_limit(0)
return o
