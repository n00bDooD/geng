physics.set_drag(0.9);
physics.set_gravity(vector.new(0, -98.1 * 4));

loadfile('data/utils/tiled_loader.lua')()

load_tiled_file(loadfile('data/maps.d/test_map.lua')())

physics.reindex_static()

