#include "inputaxis.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define TOTALLY_UNUSED_NAME "__deleted__"

inputaxis* find_axis(inputaxis_data* da, const char* name)
{
	size_t idx = 0;
	for(idx = 0; idx <= da->num_inputaxes; idx++){
		if(idx == da->num_inputaxes || strcmp(name, da->axes[idx].name) == 0){
			break;
		}
	}

	if(idx != da->num_inputaxes) {
		return &(da->axes[idx -1]);
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

input* create_inputaxis(inputaxis_data* d)
{
	input* i = (input*)malloc(sizeof(input));
	i->input_data = d;
	i->get_input = &get_input_for_axis;
	return i;
}

inputaxis_data* delete_inputaxis(input* i)
{
	return (inputaxis_data*)i->input_data;
	free(i);
}
