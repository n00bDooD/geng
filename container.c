#include "container.h"

#include <string.h>
#include <assert.h>

container* create_container(size_t object_pool_size)
{
	container* cont = (container*)calloc(1, sizeof(container));
	assert(cont != NULL);

	cont->object_root = create_root(object_pool_size);
	assert(cont->object_root != NULL);

	cont->num_objects = object_pool_size;

	cont->num_meshes = 0;
	cont->meshes = NULL;

	cont->num_materials = 0;
	cont->materials = NULL;

	return cont;
}

void delete_container(container* c)
{
	delete_root(c->object_root);

	if (c->meshes != NULL) free(c->meshes);
	if (c->materials != NULL) free(c->materials);

	free(c);
}

void add_mesh(container* cont, mesh* add)
{
	mesh* ptr = (mesh*)realloc(cont->meshes, sizeof(mesh) * ++cont->num_meshes);
	assert(ptr != NULL);
	cont->meshes = ptr;

	mesh* to = &(cont->meshes[cont->num_meshes-1]);
	memcpy(to, add, sizeof(mesh));
}

void add_material(container* cont, material* add)
{
	material* ptr = (material*)realloc(cont->materials, sizeof(material) * ++cont->num_materials);
	assert(ptr != NULL);
	cont->materials = ptr;

	material* to = &(cont->materials[cont->num_materials-1]);
	memcpy(to, add, sizeof(material));
}

