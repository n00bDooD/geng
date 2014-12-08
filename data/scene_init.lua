scene.load_prefab_file("data/prefabs.d/ball.prefab.lua")
scene.load_prefab_file("data/prefabs.d/glass_block.prefab.lua")
scene.load_prefab_file("data/prefabs.d/metal.prefab.lua")
scene.load_prefab_file("data/prefabs.d/block.prefab.lua")
scene.load_prefab_file("data/prefabs.d/coin.lua")
scene.load_prefab_file("data/prefabs.d/hip_to_be_square.lua")

scene.load_behaviour_file("data/behaviour.d/controllable.lua")
scene.load_behaviour_file("data/behaviour.d/upfloat.lua")
scene.load_behaviour_file("data/behaviour.d/timeout.lua")
scene.load_behaviour_file("data/behaviour.d/collision_logger.lua")
scene.load_behaviour_file("data/behaviour.d/boolet.lua")
scene.load_behaviour_file("data/behaviour.d/breakable.lua")

local block = scene.spawn_prefab("block")
block:set_pos(200, -200)

block:set_angle(1)

local ball = scene.spawn_prefab("ball")
ball:set_pos(200, -70)

local glass = scene.spawn_prefab("glass_block")
glass:set_pos(300, -300)

local metal = scene.spawn_prefab("metal")
metal:set_pos(100, -300)

local target = scene.spawn_prefab("hip_to_be_square")
target:set_pos(350, -70)
