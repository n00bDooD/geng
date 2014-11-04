#include "object.h"

#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <errno.h>

#define error(x) fprintf(stderr, "%s:%i : %s\n", __FILE__, __LINE__, x)
#define prerror() error(strerror(errno))

#define POISON (void*)0xDEADBABE

object* get_first_inactive(object* pool)
{
	static object* cache4pool;
	static int cacheidx;

	if (cache4pool == pool
	    && pool[cacheidx].active == false
	    && pool[cacheidx-1].active == true) {
		return &(pool[cacheidx++]);
	}

	size_t idx = 0;
	while(pool[idx].active == true && pool[idx+1].children != POISON) {
		if (!pool[idx+1].active) {
			cache4pool = pool;
			cacheidx = idx+2;

			return &(pool[idx+1]);
		}
		idx++;
	}

	return POISON;
}

object* create_root(size_t poolsize)
{
	object* new_pool = (object*)malloc((poolsize + 1) * sizeof(object));
	if (new_pool == NULL) {
		prerror();
		return NULL;
	}

	transform* new_transform_pool = (transform*)malloc(poolsize * sizeof(transform));
	if (new_transform_pool == NULL) {
		prerror();
		free(new_pool);
		new_pool = NULL;
		return NULL;
	}

	for(size_t i = 0; i < poolsize; i++) {
		new_pool[i].active = false;
		new_pool[i].transform = &(new_transform_pool[i]);

		new_pool[i].parent = NULL;
		new_pool[i].nchildren = 0;
		new_pool[i].children = NULL;

		new_pool[i].objpool = new_pool;
	}

	new_pool[0].active = true;

	new_pool[poolsize].children = POISON;
	new_pool[poolsize].parent = POISON;
	new_pool[poolsize].transform = POISON;

	return new_pool;
}

void delete_root(object* root)
{
	while(root->nchildren > 0) {
		delete_object(root->children[root->nchildren - 1]);
	}
	free(root->transform);
	free(root);
}

object* create_object(object* parent)
{
	object* new_obj = get_first_inactive(parent->objpool);

	new_obj->active = true;
	new_obj->parent = parent;
	new_obj->nchildren = 0;

	object** newp = (object**)realloc(parent->children, sizeof(object*) * ++parent->nchildren);
	if(newp == NULL) {
		prerror();
		free(parent->children);
		parent->children = NULL;
		return NULL;
	}
	parent->children = newp;
	parent->children[parent->nchildren-1] = new_obj;

	return new_obj;
}

void delete_object(object* ob)
{
	while(ob->nchildren > 0) {
		delete_object(ob->children[ob->nchildren - 1]);
	}
	if(ob->children != NULL) {
		free(ob->children);
	}
	ob->children = NULL;

	ob->parent->nchildren--;
	/* Get idx of the ptr to this obj */
	int idx = -1;
	while(ob->parent->children[++idx] != ob);

	/* Swap with last obj */
	ob->parent->children[idx] = ob->parent->children[ob->parent->nchildren];

	/* Reallocate so last obj is 'freed' */
	object** newp = (object**)realloc(ob->parent->children, sizeof(object*) * ob->parent->nchildren);
	if(newp == NULL && errno != 0) {
		prerror();
		free(ob->parent->children);
		ob->parent->children = NULL;
		return;
	}
	ob->parent->children = newp;

	ob->active = false;
	ob->parent = NULL;

	bzero(ob->transform, sizeof(transform));
}

