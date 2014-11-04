#ifndef OBJ_H
#define OBJ_H

#include <stdlib.h>
#include <stdbool.h>

typedef struct {
    float x;
    float y;
} vec2;

typedef struct {
    float x;
    float y;
    float z;
} vec3;

typedef struct {
    float x;
    float y;
    float z;
    float w;
} vec4;

typedef struct {
    vec4 position;
    vec4 rotation;
} transform;

typedef struct object object;

struct object {
    bool active;
    transform* transform;

    object* parent;

    size_t nchildren;
    object** children;

    object* objpool;
};

object* create_root(size_t pool_size);
void delete_root(object* root);

object* create_object(object* parent);
void delete_object(object* ob);

#endif /* OBJ_H */
