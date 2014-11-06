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
	if(o->transform_type == 1) {
		return cpBodyGetPos(o->transform.rigidbody);
	} else {
		return o->transform.transform->position;
	}
}
cpFloat get_object_angle(object* o)
{
	if(o->transform_type == 1) {
		return cpBodyGetAngle(o->transform.rigidbody);
	} else {
		return o->transform.transform->angle;
	}
}
