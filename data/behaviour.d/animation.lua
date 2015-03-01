local animations = args[1]
local current_animation = nil
if args[2] ~= nil then
	current_animation = animations[args[2]]
end 


function validate_anim(a)
	if a == nil then return false end
	if a.frames == nil then return false end
	if a.speed == nil then return false end
end

function empty_anim(name)
	local ret = {};
	ret.frames = {}
	ret.speed = 1
	ret.name = name
	return ret
end

function receive(obj, message)
	if message == nil then return end
	if type(message) == 'string' then
		local new_anim = animations[message];
		if new_anim ~= current_animation then
			current_animation = new_anim
			anim_idx = 1
		end
	else
		if validate_anim(message) then
			animations[message.name] = message
		end
	end
end

local anim_idx = 1

function scene_update(obj, step)
	if current_animation == nil then return end
	if #current_animation.frames == 0 then
		return
	end

	anim_idx = anim_idx + step * current_animation.speed
	if anim_idx >= #current_animation.frames then
		anim_idx = 1 + (anim_idx - #current_animation.frames)
	end
	local s = current_animation.frames[math.min(math.ceil(anim_idx), #current_animation.frames)]
	if s ~= nil then 
		obj:set_sprite(s.sprite)
		if s.fliph ~= nil then obj:set_fliph(s.fliph) end
		if s.flipv ~= nil then obj:set_flipv(s.flipv) end
	end
end
