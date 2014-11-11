#include "sdl_renderer.h"

void draw_objects(scene* sc)
{
	sdl_renderer* r = sc->render_data;
	for(size_t i = 0; i < sc->num_objects; ++i) {
		object* o = &(sc->pool[i]);
		if(o->flags & (OBJ_ACTIVE) && o->sprite > 0) {
			sprite* s = r->sprites + o->sprite-1;
			if(s->tex == 0) continue;
			SDL_Texture* t = (void*)r->textures + s->tex - 1;

			double a = get_object_angle(o);
			double x = get_object_posx(o) + s->offset_x;
			double y = get_object_posy(o) + s->offset_y;
			SDL_Rect dst;
			dst.x = floor(x);
			dst.y = floor(y);
			SDL_QueryTexture(t, NULL, NULL, &dst.w, &dst.h);

			SDL_RenderCopyEx(r->rend,
					 t,
					 NULL,
					 &dst,
					 a,
					 NULL,
					 SDL_FLIP_HORIZONTAL);
		}
	}
}
