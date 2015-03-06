function resolve_gid(map, gid)
	for local ts = 0, #map.tilesets do
		local tileset = map.tilesets[ts]
		local idx = (gid - tileset.firstgid)+1

		if idx < 1 then
			-- Not this one
		elseif idx > #tileset.tiles then
			-- woo, this one
			return tileset.tiles[idx]
		end
	end
end

function add_tilelayer(map, layer)
	if layer.type ~= 'tilelayer' then return end

	for local col = 1, layer.width do
		for local row = 1, layer.height do
			local tileid = layer.data[col + row]
			if tileid > 0 then
				local x = col * map.tilewidth
				local y = row * map.tileheight

				local newtile = scene.spawn_prefab(resolve_gid(map, tileid))
				newtile:set_pos(x, y)
			end
		end
	end
end

function load_tiled_file(output)
	for local lay = 0, #output.layers do
		add_tilelayer(output, output.layers[lay])
	end
end

function newtile(sprite)
	return
	function()
		local o = scene.new_object()
		o:set_sprite(sprite)
		return o
	end
end

function transform_tile(t)
	local tex = renderer.load_texture(t.image);
	local sprite = renderer.add_sprite(tex, t.width * 0.5, t.height * 0.5, 0, 0, t.width, t.height)
	return newtile(sprite)
end

