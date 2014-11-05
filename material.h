#ifndef MATERIAL_H
#define MATERIAL_H

/*
 * A material is a shader + a texture.
 * A texture may be shared by several materials, just as a shader can
 * be shared among materals.
 * In the same manner, a single material can be shared across models.
 * A model is a mesh + a material.
 */

typedef struct {
	void* shader;
	void* texture;
} material;

#endif /* MATERIAL_H */
