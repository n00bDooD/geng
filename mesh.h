#ifndef MESH_H
#define MESH_H

#include <GL/glew.h>

typedef struct {
	size_t vertex_data_length;
	GLfloat vertex_data;

	size_t indices_length;
	GLuint indices;
} mesh;

#endif /* MESH_H */
