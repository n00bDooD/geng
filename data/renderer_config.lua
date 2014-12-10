function add_sprites(thandle, sprites)
	for k, s in ipairs(sprites) do
		renderer.add_sprite(thandle, s.w * 0.5, s.h* 0.5, s.x, s.y, s.w, s.h)
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

add_sprites(
	renderer.add_texture("kenney/platformer/Enemies/enemies_spritesheet.tga"),
	{
		-- 49 blocker body
		{x=203, y=0, w=51, h=51},
		-- 50 blocker mad
		{x=136, y=66, w=51, h=51},
		-- 51 blocker sad
		{x=188, y=66, w=51, h=51},
		-- 52 fish dead
		{x=0, y=69, w=66, h=42},
		-- 53 fish swim 1
		{x=76, y=0, w=66, h=42},
		-- 54 fish swim 2
		{x=73, y=43, w=62, h=43},
		-- 55 fly dead
		{x=143, y=0, w=59, h=33},
		-- 56 fly 1
		{x=0, y=32, w=72, h=36},
		-- 57 fly 2
		{x=0, y=0, w=75, h=31},
		-- 58 pokermad
		{x=255, y=0, w=48, h=146},
		-- 59 pokersad
		{x=304, y=0, w=48, h=146},
		-- 60 slime dead
		{x=0, y=112, w=59, h=12},
		-- 61 slime walk 1
		{x=52, y=125, w=50, h=28},
		-- 62 slime walk 2
		{x=0, y=125, w=51, h=26},
		-- 63 snailshell
		{x=103, y=119, w=44, h=30},
		-- 64 snailshell upsidedown
		{x=148, y=118, w=44, h=30},
		-- 65 snailwalk1
		{x=143, y=34, w=54, h=31},
		-- 66 snailwalk2
		{x=67, y=87, w=57, h=31}
	}
)
