physics.set_drag(0.9);
physics.set_gravity(vector.new(0, -98.1 * 4));

scene.load_prefab("hero", loadfile("data/prefabs.d/hero.lua"))
scene.load_prefab("buddy", loadfile("data/prefabs.d/buddy.lua"))
scene.load_prefab("baddie", loadfile("data/prefabs.d/baddie.lua"))
scene.load_prefab("mad_block", loadfile("data/prefabs.d/mad_block.lua"))

scene.load_behaviour("controllable", loadfile("data/behaviour.d/controllable.lua"))
scene.load_behaviour("platformer", loadfile("data/behaviour.d/platformer.lua"))
scene.load_behaviour("animation", animation.load())
scene.load_behaviour("cfunctest", call_logger.load())

local hero = scene.spawn_prefab("hero")
hero:set_pos(200, -200)


local buddy = scene.spawn_prefab("buddy")
buddy:set_pos(300, -200)

local baddie = scene.spawn_prefab("baddie")
baddie:set_pos(400, -200)

local b = 0
for i = 0, 20 do
	local blocker = scene.spawn_prefab('mad_block')
	b = b + 0.25
	
	blocker:set_pos(51 * i, (-600) + (51 * math.floor(b)))
end

physics.reindex_static()

