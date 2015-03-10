
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

	local col = 0
	local row = 0
	for i, b in ipairs(layer.data) do
		col = i - (row * layer.width)
		if col > layer.width then
			row = row + 1
			col = 1
		end
		
		if b > 0 then
			local x = (col-1) * map.tilewidth
			local y = (row-1) * map.tileheight

			local newtile = scene.spawn_prefab(resolve_gid(map, b))
			if layer.visible == false then newtile:set_sprite(0) end
			newtile:set_pos(x, 200 - y)
		end
	end

	do return end
	for col = 0, layer.width -1 do
		for row = 0, layer.height -1 do
			local tileid = layer.data[(row*layer.height) + col +1]
			if tileid ~= nil and tileid > 0 then
				local x = col * map.tilewidth
				local y = row * map.tileheight

				local newtile = scene.spawn_prefab(resolve_gid(map, tileid))
				if layer.visible == false then newtile:set_sprite(0) end
				newtile:set_pos(x, 600 - y)
			end
		end
	end
end


--
-- Transformation functions (load prefabs for tiles etc)
--

function transform_tileset(ts)
	for _, tile in ipairs(ts.tiles) do
		scene.load_prefab(ts.name .. '_' .. tostring(tile.id), transform_tile(tile))
	end
end

function transform_tile(t)
	local tex = renderer.add_texture(t.image);
	local sprite = renderer.add_sprite(tex, t.width * 0.5, t.height * 0.5, 0, 0, t.width, t.height)
	if t.properties == nil then t.properties = {} end

	local collider = function(o) return nil end
	if t.properties.collider ~= nil and t.properties.collider == 'true' then
		collider = function(o) o:add_box_collider(t.width, t.height) end
	elseif t.objectGroup ~= nil then
		for _, o in ipairs(t.objectGroup.objects) do
			if o.shape == 'rectangle' then
				local xpos = o.x
				local ypos = o.y
				local edges = {
					      vector.new(xpos, ypos),
					      vector.new(xpos + o.width, ypos),
					      vector.new(xpos + o.width, ypos - o.height),
					      vector.new(xpos, ypos - o.height),
					      }
				collider = function(o) o:add_poly_collider(edges, vector.new(xpos, (t.height*0.5) - ypos)) end
			end
		end
	end
	return (function() 
			local o = scene.newobject()
		 	o:set_sprite(sprite)
			collider(o)
		 	return o
	 	end)
end


function load_tiled_file(output)
	for _, ts in ipairs(output.tilesets) do
		transform_tileset(ts)
	end
	for _, l in ipairs(output.layers) do
		add_tilelayer(output, l)
	end
end

