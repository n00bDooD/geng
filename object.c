#include "object.h"
#include "services.h"

void draw_objects(size_t objc, object* obj)
{
	renderer* r = services_get_renderer();

	for(size_t idx = 0; idx < objc; idx++) {
		r->draw_object(r->render_data, &(obj[idx]));
	}
}

cpVect get_object_position(object* o)
{
	if(o->transform_type == 0) {
		return cpBodyGetPos(o->transform.rigidbody);
	} else {
		return o->transform.transform->position;
	}
}
cpFloat get_object_angle(object* o)
{
	if(o->transform_type == 0) {
		cpFloat a = cpBodyGetAngle(o->transform.rigidbody);
		return a;
	} else {
		return o->transform.transform->angle;
	}
}

void update_objects(size_t objc, object* objects)
{
	for(size_t i = 0; i < objc; ++i) {
		if(objects[i].update == NULL) continue;
		objects[i].update(&(objects[i]));
	}
}
