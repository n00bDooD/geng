#include "sdl_renderer.h"
#include "../global.h"
#include <sys/stat.h>
#include <fcntl.h>
#include "tga.h"
#include <stdio.h>


void flip_tga_vertical(targa_file* tga);

texhandle sdl_renderer_add_texture(sdl_renderer* r, SDL_Texture* tex)
{
	void* n = realloc(r->textures, (++r->num_textures) * sizeof(SDL_Texture*));
	if(n == NULL)
		error("add_texture");

	r->textures = n;
	r->textures[r->num_textures-1] = tex;
	return (texhandle)r->num_textures;
}

spritehandle sdl_renderer_add_sprite(sdl_renderer* r, texhandle tex, int xoffset,
		int yoffset, int srcx, int srcy, int srcw, int srch)
{
	void* n = realloc(r->sprites, (++r->num_sprites) * sizeof(sprite));
	if (n == NULL)
		error("add_sprite");
	r->sprites = n;

	sprite* s = r->sprites + r->num_sprites-1;
	s->tex = tex;
	s->offset_x = xoffset;
	s->offset_y = yoffset;
	if (srcx == -1 && srcy == -1 && srcw == -1 && srch == -1) {
		s->tex_source_x = 0;
		s->tex_source_y = 0;
		if(SDL_QueryTexture(r->textures[tex-1],
					NULL, NULL,
					&(s->tex_source_w),
					&(s->tex_source_h)) < 0){
			return 0;
		}
	} else {
		s->tex_source_x = srcx;
		s->tex_source_y = srcy;
		s->tex_source_w = srcw;
		s->tex_source_h = srch;
	}
	
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
	sdl_renderer* r = get_scene_renderer(sc);
	SDL_Rect viewport; SDL_RenderGetViewport(r->rend, &viewport);
	if(r->background != 0) {
		SDL_RenderCopy(r->rend, r->textures[r->background-1],
				&viewport, &viewport);
	}

	for(size_t i = 0; i < sc->num_objects; ++i) {
		object* o = &(sc->pool[i]);
		if(o->flags & (OBJ_ACTIVE) && o->sprite > 0) {
			sprite* s = r->sprites + o->sprite-1;
			if(s->tex == 0) continue;

			SDL_RendererFlip f = SDL_FLIP_NONE;
			if (o->flags & OBJ_FLIPHOR) f |= SDL_FLIP_HORIZONTAL;
			if (o->flags & OBJ_FLIPVERT) f |= SDL_FLIP_VERTICAL;

			SDL_Texture* t = r->textures[s->tex-1];

			SDL_Rect src = {.x = s->tex_source_x,
					.y = s->tex_source_y,
					.w = s->tex_source_w,
					.h = s->tex_source_h };

			double a = get_object_angle(o) * (180/M_PI);
			double x = get_object_posx(o) * r->cam.scale;
			double y = get_object_posy(o) * r->cam.scale;
			double camx = x + r->cam.x;
			double camy = -y + r->cam.y;
			/* Avoid overflowing the integer coordinates */
			if (camx > INT_MAX || camx < INT_MIN
			    || camy > INT_MAX || camy < INT_MIN) {
				continue;
			}

			SDL_Rect dst;
			dst.x = (int)lrint(camx);
			dst.y = (int)lrint(camy);
			dst.w = src.w;
			dst.h = src.h;
			dst.w *= r->cam.scale;
			dst.h *= r->cam.scale;
			dst.x += s->offset_x * r->cam.scale;
			dst.y -= s->offset_y * r->cam.scale;

			SDL_RenderCopyEx(r->rend,
					 t,
					 &src,
					 &dst,
					 -round(a),
					 NULL,
					 f);
		}
	}
}
