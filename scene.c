#include "scene.h"
#include "global.h"
#include "lua/globlua.h"
#include "lua/lua_collision.h"
#include "lua/lua_object.h"

void cpShape_deleter(cpBody* b, cpShape* s, void* d);
void free_physics(object* o);
void run_delete_method(object*, lua_State*, const char*);

object* get_first_unused(scene* s)
{
	size_t i = 0;
	while(i < s->num_objects) {
		if (!s->pool[i].flags & OBJ_ACTIVE) {
			return &(s->pool[i]);
		}
		i++;
	}
	return NULL;
}

object* create_object(scene* s)
{
	object* o = get_first_unused(s);
	o->parent = s;
	o->physics = cpBodyNewStatic();
	cpBodySetUserData(o->physics, o);
	o->flags |= OBJ_ACTIVE;
	o->messaging = msgq_create(NULL, 1024, 1024, 128, 128);
	return o;
}

 /* Delete */

void cpShape_deleter(cpBody* b, cpShape* s, void* d)
{
	UNUSED(b);
	cpSpaceRemoveShape(d, s);
	cpShapeFree(s);
}

void free_physics(object* o)
{
	scene* s = o->parent;
	void* scene_physics = get_scene_physics(s);
	if (scene_physics == NULL) return;

	cpBodyEachShape(o->physics,
			&cpShape_deleter, scene_physics);

	if(!cpBodyIsStatic(o->physics)) {
		cpSpaceRemoveBody(scene_physics, o->physics);
	}
	cpBodyFree(o->physics);
}

void run_delete_method(object* o, lua_State* l, const char* bname)
{
	lua_getglobal(l, BEHAVIOUR_DELETE_MNAME);
	if (lua_isnil(l, -1)) {
		lua_pop(l, 1);
		return;
	}
	luaG_pushobject(l, o);
	int result = luaG_pcall(l, 1, 0);
	plua_error(l, result, bname);
}

void delete_object(scene* s, object* o)
{
	UNUSED(s);

	behaviour* obj_threads = (behaviour*)o->tag;
	size_t num_behaviours = 0;
	if(obj_threads != NULL) {
		while(obj_threads[num_behaviours].name != NULL) {
			if(obj_threads[num_behaviours].script_behaviour) {
				lua_State* l = obj_threads[num_behaviours].content.thread;
				run_delete_method(o, l, obj_threads[num_behaviours].name);
				lua_close(l);
			} else {
				call_delete(obj_threads[num_behaviours].content.beh, o);
				free(obj_threads[num_behaviours].content.beh);
			}
			free((char*)obj_threads[num_behaviours++].name);
		}
	}
	free(obj_threads);

	free_physics(o);

	if (o->name != NULL) {
		free(o->name);
	}

	msgq_free(o->messaging);
	free(o->messaging);

	bzero(o, sizeof(object));
}

void cleanup_deleted(scene* s)
{
	for(size_t i = 0; i < s->num_objects; ++i) {
		if ((s->pool[i].flags & OBJ_DELETED) != 0) {
			delete_object(s, &(s->pool[i]));
		}
	}
}


double get_object_posx(object* o)
{
	return cpBodyGetPos(o->physics).x;
}
double get_object_posy(object* o)
{
	return cpBodyGetPos(o->physics).y;
}

cpVect get_object_pos(object* o)
{
	return cpBodyGetPos(o->physics);
}

double get_object_angle(object* o)
{
	return cpBodyGetAngle(o->physics);
}



void set_object_pos(object* o, cpVect p)
{
	cpBodySetPos(o->physics, p);
}

void set_object_angle(object* o, double a)
{
	cpBodySetAngle(o->physics, a);
}


double get_object_mass(object* o)
{
	return cpBodyGetMass(o->physics);
}

double get_object_torque(object* o)
{
	return cpBodyGetTorque(o->physics);
}

double get_object_moment(object* o)
{
	return cpBodyGetMoment(o->physics);
}

double get_object_angular_velocity(object* o)
{
	return cpBodyGetAngVel(o->physics);
}

double get_object_angular_velocity_limit(object* o)
{
	return cpBodyGetAngVelLimit(o->physics);
}

cpVect get_object_velocity(object* o)
{
	return cpBodyGetVel(o->physics);
}

double get_object_velocity_limit(object* o)
{
	return cpBodyGetVelLimit(o->physics);
}


void object_apply_force(object* o, cpVect f, cpVect offset)
{
	cpBodyApplyForce(o->physics, f, offset);
}

void object_apply_impulse(object* o, cpVect f, cpVect offset)
{
	cpBodyApplyImpulse(o->physics, f, offset);
}

void object_reset_forces(object* o)
{
	cpBodyResetForces(o->physics);
}

void disable_object_physics(object* o)
{
	if(!cpBodyIsStatic(o->physics)) {
		cpSpace* s = get_scene_physics(o->parent);
		cpSpaceRemoveBody(s, o->physics);
		cpSpaceConvertBodyToStatic(s, o->physics);
	}
}

void enable_object_physics(object* o, double mass, double moment)
{
	if(cpBodyIsStatic(o->physics)) {
		cpSpace* s = get_scene_physics(o->parent);
		if (!mass > 0) {
			mass = INFINITY;
		}
		if (!moment > 0) {
			moment = INFINITY;
		}
		cpSpaceConvertBodyToDynamic(s, o->physics, mass, moment);
		cpSpaceAddBody(s, o->physics);
	}
}

cpVect object_convert_world2localpos(object* o, cpVect v)
{
	return cpBodyWorld2Local(o->physics, v);
}

cpVect object_convert_local2worldpos(object* o, cpVect v)
{
	return cpBodyLocal2World(o->physics, v);
}

void* get_scene_property(scene* s, const char* property)
{
	luaG_getreg(s->engine, property);
	if (lua_isnil(s->engine, -1)) {
		lua_pop(s->engine, 1);
		return NULL;
	}
	void* ret = lua_touserdata(s->engine, -1);
	lua_pop(s->engine, 1);
	return ret;
}

void set_scene_property(scene* s, const char* property, void* p)
{
	lua_pushlightuserdata(s->engine, p);
	luaG_setreg(s->engine, property);
}


scene* create_new_scene(lua_State* l, size_t pool_size, void* render_data, void* physics_data) 
{
	scene* s = calloc(1, sizeof(scene));
	s->pool = calloc(pool_size, sizeof(object));
	if(s->pool == NULL) error("Cannot allocate object pool");
	s->num_objects = pool_size;
	s->engine = l;
	set_scene_renderer(s, render_data);
	set_scene_physics(s, physics_data);

	setup_collision(s);
	return s;
}

