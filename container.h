#ifndef CONTAINER_H
#define CONTAINER_H

#include "object.h"
#include "mesh.h"
#include "material.h"

typedef struct {
	size_t num_objects;
	object* object_root;
	
	size_t num_meshes;
	mesh* meshes;

	size_t num_materials;
	material* materials;
} container;

container* create_container(size_t object_pool_size);
void delete_container(container* cont);

void add_mesh(container* cont, mesh* add);

void add_material(container* cont, material* mat);

#endif /* CONTAINER_H */
