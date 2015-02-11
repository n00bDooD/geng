#include "services.h"
#include "global.h"

double do_nothing_input(void* data, const char* inp);
input* get_null_input(void);

double do_nothing_input(void* data, const char* inp)
{
	UNUSED(data);
	UNUSED(inp);
	return 0;
}

input* get_null_input()
{
	input* i = (input*)malloc(sizeof(input));
	if(i == NULL) error("get_null_input");
	i->input_data = NULL;
	i->get_input = &do_nothing_input;

	return i;
}

static input* cur_input = NULL;

input* services_get_input()
{
	if(cur_input == NULL) {
		services_register_input(get_null_input());
	}
	return cur_input;
}

input* services_register_input(input* i)
{
	input* old = cur_input;
	if(old != NULL && old->input_data == NULL && old->get_input == &do_nothing_input) {
		free(old);
		old = NULL;
	}
	cur_input = i;
	return old;
}
