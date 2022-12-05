#ifndef SCENE_BUFFER_H
#define SCENE_BUFFER_H

#include <stddef.h>

#define INITAL_BUFFER_SIZE 10

/* Struct equivalent to OpenGL vec4 */
typedef struct vec4 {
    float x;
    float y;
    float z;
    float w;
} vec4;

/* Struct containing the object making up the scene */
typedef struct scene_buffer_t scene_buffer_t;

/* Scene buffer functions */
scene_buffer_t *new_buffer();
void            buffer_add(scene_buffer_t *scene_buffer, vec4 obj);
size_t          buffer_count_elements(scene_buffer_t *scene_buffer);
void           *buffer_pointer(scene_buffer_t *scene_buffer, int *size);

#endif /* SCENE_BUFFER_H */
