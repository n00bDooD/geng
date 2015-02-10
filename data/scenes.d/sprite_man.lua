physics.set_drag(0.9);
physics.set_gravity(vector.new(0, -98.1 * 4));

scene.load_prefab_file("hero", loadfile("data/prefabs.d/hero.lua"))
scene.load_prefab_file("buddy", loadfile("data/prefabs.d/buddy.lua"))
scene.load_prefab_file("baddie", loadfile("data/prefabs.d/baddie.lua"))
scene.load_prefab_file("fly", loadfile("data/prefabs.d/fly.lua"))

scene.load_behaviour_file("controllable", loadfile("data/behaviour.d/controllable.lua"))
scene.load_behaviour_file("platformer", loadfile("data/behaviour.d/platformer.lua"))
scene.load_behaviour_file("boolet", loadfile("data/behaviour.d/boolet.lua"))
scene.load_behaviour_file("character_anim", loadfile("data/behaviour.d/character_anim.lua"))

local hero = scene.spawn_prefab("hero")
hero:set_pos(200, -200)

local buddy = scene.spawn_prefab("buddy")
buddy:set_pos(300, -200)

local baddie = scene.spawn_prefab("baddie")
baddie:set_pos(400, -200)

for i = 0, 20 do
	local blocker = scene.spawn_prefab('fly')
	blocker:set_pos(51 * i, -600)
end

physics.reindex_static()

