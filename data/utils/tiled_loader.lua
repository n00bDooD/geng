
-- Resolve a tile data-value to the name of a brefab
function resolve_gid(map, gid)
	for _, tileset in ipairs(map.tilesets) do
		local idx = gid - tileset.firstgid +1

		if idx < 1 then
			-- Not this one
		elseif idx <= #tileset.tiles then
			-- woo, this one
			return tileset.name .. '_' .. tileset.tiles[idx].id
		end
	end
end

-- Add a tilelayer
function add_tilelayer(map, layer)
	if layer.type ~= 'tilelayer' then return end

	for col = 0, layer.width -1 do
		for row = 0, layer.height -1 do
			local tileid = layer.data[(row*layer.height) + col +1]
			if tileid ~= nil and tileid > 0 then
				local x = col * map.tilewidth
				local y = 200 - row * map.tileheight

				local newtile = scene.spawn_prefab(resolve_gid(map, tileid))
				if layer.visible == false then newtile:set_sprite(0) end
				newtile:set_pos(x, y)
			end
		end
	end
end


--
-- Transformation functions (load prefabs for tiles etc)
--

function transform_tileset(ts)
	for i = 1, #ts.tiles do
		local tile = ts.tiles[i]
		scene.load_prefab(ts.name .. '_' .. tostring(tile.id), transform_tile(tile))
	end
end

function transform_tile(t)
	local tex = renderer.add_texture(t.image);
	local sprite = renderer.add_sprite(tex, t.width * 0.5, t.height * 0.5, 0, 0, t.width, t.height)
	return (function() local o = scene.newobject(); o:set_sprite(sprite); return o end)
end


function load_tiled_file(output)
	for _, ts in ipairs(output.tilesets) do
		transform_tileset(ts)
	end
	for _, l in ipairs(output.layers) do
		add_tilelayer(output, l)
	end
end

