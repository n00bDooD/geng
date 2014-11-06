#include "sdl_renderer.h"
#include <chipmunk/chipmunk.h>

void sdl_renderer_draw(void* renderer_data, object* obj)
{
	if(obj->sprite == NULL) return;
	sdl_renderer_data* r = (sdl_renderer_data*)renderer_data;

	cpVect position = get_object_position(obj);
	cpFloat angle = get_object_angle(obj);

	/* SDL takes angles in degrees, we do radians. */
	double angle_deg = angle * (180.0/M_PI);
	SDL_Rect rec;
	rec.x = round(position.x);
	rec.y = round(position.y);
	SDL_QueryTexture(obj->sprite, NULL, NULL, &rec.w, &rec.h);

	SDL_RenderCopyEx(r->r,
	                 obj->sprite,
	                 NULL,
	                 &rec,
	                 angle_deg,
	                 NULL,
	                 SDL_FLIP_NONE);
}

renderer* sdl_renderer_create(SDL_Window* w, SDL_Renderer* r)
{
	sdl_renderer_data* dat = (sdl_renderer_data*)malloc(sizeof(sdl_renderer_data));
	renderer* re = (renderer*)malloc(sizeof(renderer));

	dat->w = w;
	dat->r = r;

	re->render_data = dat;
	re->draw_object = &sdl_renderer_draw;
	return re;
}

void sdl_renderer_delete(renderer* r)
{
	sdl_renderer_data* da = (sdl_renderer_data*)r->render_data;

	free(da);
	free(r);
}
