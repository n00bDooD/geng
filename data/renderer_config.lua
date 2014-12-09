function add_sprites(thandle, sprites)
	for k, s in ipairs(sprites) do
		renderer.add_sprite(thandle, s.w * 0.5, s.h, s.x, s.y, s.w, s.h)
	end
end

add_sprites(
	renderer.add_texture("kenney/platformer/Player/p1_spritesheet.tga"),
	{
		{x=365, y=98, w=69, h=71},
		{x=0, y=196, w=66, h=92},
		{x=438, y=0, w=69, h=92},
		{x=438, y=93, w=67, h=94},
		{x=67, y=196, w=66, h=92},
		{x=0, y=0, w=72, h=97},
		{x=73, y=0, w=72, h=97},
		{x=146, y=0, w=72, h=97},
		{x=0, y=98, w=72, h=97},
		{x=73, y=98, w=72, h=97},
		{x=146, y=98, w=72, h=97},
		{x=219, y=0, w=72, h=97},
		{x=292, y=0, w=72, h=97},
		{x=219, y=98, w=72, h=97},
		{x=365, y=0, w=72, h=97},
		{x=292, y=98, w=72, h=97}
	}
)


add_sprites(
	renderer.add_texture("kenney/platformer/Player/p2_spritesheet.tga"),
	{
		{x=355, y=95, w=67, h=72},
		{x=0, y=190, w=66, h=92},
		{x=426, y=0, w=67, h=92},
		{x=423, y=95, w=66, h=94},
		{x=67, y=190, w=66, h=92},
		{x=0, y=0, w=70, h=94},
		{x=71, y=0, w=70, h=94},
		{x=142, y=0, w=70, h=94},
		{x=0, y=95, w=70, h=94},
		{x=71, y=95, w=70, h=94},
		{x=142, y=95, w=70, h=94},
		{x=213, y=0, w=70, h=94},
		{x=284, y=0, w=70, h=94},
		{x=213, y=95, w=70, h=94},
		{x=355, y=0, w=70, h=94},
		{x=284, y=95, w=70, h=94}
	}
)

add_sprites(
	renderer.add_texture("kenney/platformer/Player/p3_spritesheet.tga"),
	{
		{x=365, y=98, w=69, h=71},
		{x=0, y=196, w=66, h=92},
		{x=438, y=0, w=69, h=92},
		{x=438, y=93, w=67, h=94},
		{x=67, y=196, w=66, h=92},
		{x=0, y=0, w=72, h=97},
		{x=73, y=0, w=72, h=97},
		{x=146, y=0, w=72, h=97},
		{x=0, y=98, w=72, h=97},
		{x=73, y=98, w=72, h=97},
		{x=146, y=98, w=72, h=97},
		{x=219, y=0, w=72, h=97},
		{x=292, y=0, w=72, h=97},
		{x=219, y=98, w=72, h=97},
		{x=365, y=0, w=72, h=97},
		{x=292, y=98, w=72, h=97}
	}
)
