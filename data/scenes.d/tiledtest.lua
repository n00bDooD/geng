physics.set_drag(0.9);
physics.set_gravity(vector.new(0, -98.1 * 4));


loadfile('data/utils/tiled_loader.lua')()

load_tiled_file(loadfile('data/maps.d/test_map.lua')())

scene.load_prefab("hero", loadfile("data/prefabs.d/hero.lua"))
scene.load_behaviour("platformer", loadfile("data/behaviour.d/platformer.lua"))
scene.load_behaviour("animation", animation.load())
local hero = scene.spawn_prefab("hero")
hero:set_pos(200, -200)

physics.reindex_static()

