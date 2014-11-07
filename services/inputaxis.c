#include "inputaxis.h"
#include <stdlib.h>
#include <string.h>

double get_input_for_axis(void* data, const char* axis)
{
	inputaxis_data* da = (inputaxis_data*)data;

	size_t idx = 0;
	for(idx = 0; idx <= da->num_inputaxes; idx++){
		if(idx == da->num_inputaxes || strcmp(axis, da->axes[idx].name) == 0){
			break;
		}
	}
	if(idx != da->num_inputaxes) {
		return da->axes[idx -1].value;
	} else {
		return 0;
	}
}

input* create_inputaxis()
{
	input* i = (input*)malloc(sizeof(input));
	i->input_data = NULL;
	i->get_input = &get_input_for_axis;
	return i;
}

void delete_inputaxis(input* i)
{
	free(i);
}
