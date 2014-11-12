#include "cp_physics.h"
#include <chipmunk/chipmunk.h>

cpBody* create_physics(scene* s, object* o, double mass, double moment)
{
	cpSpaceAddBody(s->physics_data, cpBodyNew(mass, moment));
}
