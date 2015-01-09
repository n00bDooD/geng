scene.load_prefab_file("data/prefabs.d/hero.lua")
scene.load_prefab_file("data/prefabs.d/buddy.lua")
scene.load_prefab_file("data/prefabs.d/baddie.lua")
scene.load_prefab_file("data/prefabs.d/fly.lua")

scene.load_behaviour_file("data/behaviour.d/controllable.lua")
scene.load_behaviour_file("data/behaviour.d/platformer.lua")
scene.load_behaviour_file("data/behaviour.d/boolet.lua")
scene.load_behaviour_file("data/behaviour.d/character_anim.lua")

local hero = scene.spawn_prefab("hero")
hero:set_pos(200, -200)

local buddy = scene.spawn_prefab("buddy")
buddy:set_pos(300, -200)

local baddie = scene.spawn_prefab("baddie")
baddie:set_pos(400, -200)

for i = 0, 50 do
	local blocker = scene.spawn_prefab('fly')
	blocker:set_pos(51 * i, -600)
end
