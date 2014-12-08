tex_handle = renderer.add_texture("kenney/tga/Aliens/alienBlue_round.tga")
renderer.add_sprite(tex_handle, -35, -35)
	
tex_handle = renderer.add_texture("kenney/tga/Wood elements/elementWood011.tga")
renderer.add_sprite(tex_handle, -70, -35)

tex_handle = renderer.add_texture("kenney/tga/Metal elements/elementMetal010.tga")
renderer.add_sprite(tex_handle, -35, -35)

tex_handle = renderer.add_texture("kenney/tga/Glass elements/elementGlass014.tga")
renderer.add_sprite(tex_handle, -110, -35)

tex_handle = renderer.add_texture("kenney/tga/Backgrounds/colored_grass.tga")
renderer.set_blendmode(tex_handle, 0);
renderer.set_background(tex_handle)

tex_handle = renderer.add_texture("kenney/tga/Other/coinGold.tga")
renderer.add_sprite(tex_handle, -35, -35)

tex_handle = renderer.add_texture("kenney/tga/Aliens/alienGreen_square.tga")
renderer.add_sprite(tex_handle, -35, -35)

tex_handle = renderer.add_texture("kenney/tga/Wood elements/elementWood014.tga")
renderer.add_sprite(tex_handle, -70, -35)

tex_handle = renderer.add_texture("kenney/tga/Wood elements/elementWood046.tga")
renderer.add_sprite(tex_handle, -70, -35)

--renderer.set_cam_scale(2)
renderer.set_cam_pos(3,-80)
