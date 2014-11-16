#include "sdl_renderer.h"
#include "../global.h"
#include <sys/stat.h>
#include <fcntl.h>
#include "tga.h"

texhandle sdl_renderer_add_texture(sdl_renderer* r, SDL_Texture* tex)
{
	void* n = realloc(r->textures, (++r->num_textures) * sizeof(SDL_Texture*));
	if(n == NULL)
		error("add_texture");

	r->textures = n;
	r->textures[r->num_textures-1] = tex;
	return (texhandle)r->num_textures;
}

spritehandle sdl_renderer_add_sprite(sdl_renderer* r, texhandle tex, int xoffset, int yoffset)
{
	void* n = realloc(r->sprites, (++r->num_sprites) * sizeof(sprite));
	if (n == NULL)
		error("add_sprite");
	r->sprites = n;

	sprite* s = r->sprites + r->num_sprites-1;
	s->tex = tex;
	s->offset_x = xoffset;
	s->offset_y = yoffset;
	
	return (spritehandle)r->num_sprites;
}


void flip_tga_vertical(targa_file* tga)
{
	size_t data_len = tga_get_image_buffer_length(tga);
	uint8_t* new_buf = (uint8_t*)malloc(data_len);
	if(new_buf == NULL) fprintf(stderr, "%s\n", strerror(errno));
	size_t memsize = 0;
	switch(tga->head.depth) {
		case 8:
			memsize=1; break;
		case 16:
			memsize=2; break;
		case 24:
			memsize=3; break;
		case 32:
			memsize=4; break;
		default:
			fprintf(stderr, "Invalid depth in tga-header.");
	}
	memsize *= tga->head.width;
	size_t curidx = memsize;
	while(curidx < data_len){
		memcpy(new_buf + curidx, tga->image_data + (data_len - (curidx+memsize)), memsize);
		curidx += memsize;
	}
	memcpy(new_buf, tga->image_data + data_len - memsize, memsize);
	free(tga->image_data);
	tga->image_data = new_buf;
}

SDL_Texture* create_tex_from_file(sdl_renderer* r, const char* filename)
{
	int fd = open(filename, O_RDONLY);
	if(fd == -1) error("load_tex_from_file");

	targa_file* tga = tga_readfile(fd);
	if (tga == NULL) fprintf(stderr, "%s\n", strerror(errno));
	flip_tga_vertical(tga);

	SDL_Texture* tex = SDL_CreateTexture(r->rend, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, tga->head.width, tga->head.height);
	if(tex == NULL) sdl_error("Texture creation failed.");

	if(SDL_UpdateTexture(tex, NULL, tga->image_data, tga->head.width * sizeof(char) * 4) != 0) {
		sdl_error("Texture write failed.");
	}
	if(SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND) != 0) {
		sdl_error("Setting texture blend mode failed.");
	}

	free(tga->image_data);
	free(tga);
	return tex;
}



void draw_objects(scene* sc)
{
	sdl_renderer* r = sc->render_data;
	SDL_Rect viewport; SDL_RenderGetViewport(r->rend, &viewport);
	if(r->background != 0) {
		SDL_RenderCopy(r->rend, r->textures[r->background-1], &viewport, &viewport);
	}

	for(size_t i = 0; i < sc->num_objects; ++i) {
		object* o = &(sc->pool[i]);
		if(o->flags & (OBJ_ACTIVE) && o->sprite > 0) {
			sprite* s = r->sprites + o->sprite-1;
			if(s->tex == 0) continue;
			SDL_Texture* t = r->textures[s->tex-1];

			double a = get_object_angle(o) * (180/M_PI);
			double x = get_object_posx(o) + s->offset_x;
			double y = get_object_posy(o) - s->offset_y;
			SDL_Rect dst;
			dst.x = floor(x);
			dst.y = floor(-y);
			if(SDL_QueryTexture(t, NULL, NULL, &dst.w, &dst.h) < 0){
				//sdl_error("SDL_QueryTexture");
			}

			SDL_RenderCopyEx(r->rend,
					 t,
					 NULL,
					 &dst,
					 -a,
					 NULL,
					 SDL_FLIP_NONE);
		}
	}
}
