#include "inputaxis.h"
#include "../global.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#define TOTALLY_UNUSED_NAME "__deleted__"

inputaxis* find_axis(inputaxis_data* da, const char* name);
inputaxis* get_unused_axis(inputaxis_data* d);

inputaxis* find_axis(inputaxis_data* da, const char* name)
{
	size_t idx = 0;
	for(idx = 0; idx <= da->num_inputaxes; idx++){
		if(idx == da->num_inputaxes || strcmp(name, da->axes[idx].name) == 0){
			break;
		}
	}

	if(idx != da->num_inputaxes) {
		return &(da->axes[idx]);
	} else {
		return NULL;
	}
}

double get_input_for_axis(void* data, const char* axis)
{
	inputaxis_data* da = (inputaxis_data*)data;

	inputaxis* a = find_axis(da, axis);
	if(a != NULL) {
		return a->value;
	} else {
		return 0;
	}
}

inputaxis* get_unused_axis(inputaxis_data* d)
{
	inputaxis* r = find_axis(d, TOTALLY_UNUSED_NAME);
	if(r == NULL) {
		inputaxis* swp = (inputaxis*)realloc(d->axes,
				sizeof(inputaxis) * (d->num_inputaxes + 1));
		if(swp == NULL) error("get_unused_axis realloc");
		assert(swp != NULL);
		d->axes = swp;
		r = &(d->axes[d->num_inputaxes++]);
	}
	return r;
}

int create_axis(inputaxis_data* d, const char* name, axis_config* c)
{
	if(find_axis(d, name) == NULL){
		inputaxis* new_axis = get_unused_axis(d);
		new_axis->name = name;
		new_axis->value = 0;
		new_axis->settings = c;
		return 0;
	} else {
		return -1;
	}
}

int delete_axis(inputaxis_data* d, const char* name, axis_config** c)
{
	inputaxis* a = find_axis(d, name);
	if(a == NULL) {
		return -1;
	}

	a->name = TOTALLY_UNUSED_NAME;
	a->value = 0;

	// If the caller /needs/ to know the ptr to the settings,
	// he would have set c. Don't muck with it either way..
	if(c != NULL) {
		c = &(a->settings);
	}

	free(a);
	return 0;
}

axis_config* get_axis_settings(inputaxis_data* d, const char* name) 
{
	inputaxis* a = find_axis(d, name);
	if(a == NULL) return NULL;
	return a->settings;
}

int set_axis_settings(inputaxis_data* d, const char* name, axis_config* settings)
{
	inputaxis* a = find_axis(d, name);
	if(a == NULL) return -1;
	a->settings = settings;
	return 0;
}

void reset_axis_values(inputaxis_data* d)
{
	for(size_t i = 0; i < d->num_inputaxes; ++i)
	{
		d->axes[i].value = 0;
	}
}

int update_axis_value(inputaxis_data* d, const char* name, double val)
{
	inputaxis* a = find_axis(d, name);
	if(a == NULL) return -1;

	if(a->settings != NULL) {
		axis_config* s = a->settings;

		if(!s->enabled) val = 0;
		if(val > 0 || val < 0) {
			if(s->invert) val = -val;

			val = fmax(val, -s->negative_maximum);
			val = fmin(val, s->positive_maximum);

			if(val < 0) {
				if(val > -s->negative_deadzone)
					val = 0;
			} else {
				if(val < s->positive_deadzone)
					val = 0;
			}
		}
	}

	if(val > 0 || val < 0) {
		if(a->value > 0 || a->value < 0){
			a->value = val;
		} else {
			a->value = (a->value + val) / 2.0;
		}
	}
	return 0;
}

static axis_config* def_settings = NULL;

axis_config* default_settings(){
	if (def_settings == NULL) {
		def_settings = (axis_config*)calloc(1, sizeof(axis_config));
		if(def_settings == NULL) error("default_settings");
		assert(def_settings != NULL);

		def_settings->enabled = true;
		def_settings->negative_maximum = 1;
		def_settings->positive_maximum = 1;
	}
	return def_settings;
}
