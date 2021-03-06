physics.set_drag(0.9);
physics.set_gravity(vector.new(0, -9.81));

loadfile('data/utils/tiled_loader.lua')()

load_tiled_file(loadfile('data/maps.d/test_map.lua')())

scene.load_prefab("hero", loadfile("data/prefabs.d/hero.lua"))
scene.load_behaviour("platformer", loadfile("data/behaviour.d/platformer.lua"))
scene.load_behaviour("camera_center", loadfile("data/behaviour.d/camera_center.lua"))
scene.load_behaviour("animation", animation.load())
local hero = scene.spawn_prefab("hero")
hero:add_behaviour("camera_center")
hero:set_pos(200, -200)

physics.reindex_static()

