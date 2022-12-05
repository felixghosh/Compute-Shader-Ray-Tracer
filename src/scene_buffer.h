#ifndef SCENE_BUFFER_H
#define SCENE_BUFFER_H

#include <stddef.h>

#include "linalg.h"

#define INITAL_BUFFER_SIZE 10

/* Object storing sphere properties */
typedef struct sphere_t {
    vec4 geometry; /*index (xyz) is its center, and index 3 is radius */
    vec4 color; /* only xyz will be used, is kept vec4 to make alignment easier when sending it to
           the gpu*/
} sphere_t;

/* Struct containing the object making up the scene */
typedef struct scene_buffer_t scene_buffer_t;

/* Sphere functions */
sphere_t create_sphere(float radius, vec3 center, vec3 color);

/* Scene buffer functions */
scene_buffer_t *new_buffer();
void            buffer_add(scene_buffer_t *scene_buffer, sphere_t sphere);
size_t          buffer_count_elements(scene_buffer_t *scene_buffer);
void           *buffer_pointer(scene_buffer_t *scene_buffer, int *size);

#endif /* SCENE_BUFFER_H */
