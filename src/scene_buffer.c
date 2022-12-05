#include "scene_buffer.h"

#include <stdio.h>
#include <stdlib.h>

#include "linalg.h"

/* Struct containing the object making up the scene */
struct scene_buffer_t {
    int       capacity;
    int       size;
    sphere_t *buffer;
};

sphere_t create_sphere(float radius, vec3 center, vec3 color)
{
    return (sphere_t){
        (vec4){center.x, center.y, center.z, radius},
        (vec4){color.x,  color.y,  color.z,  0.0   },
    };
}

scene_buffer_t *new_buffer()
{
    scene_buffer_t *sbo;
    size_t          size = sizeof(scene_buffer_t);

    if ((sbo = calloc(1, size)) == NULL ||
        (sbo->buffer = calloc(sizeof(sphere_t), INITAL_BUFFER_SIZE)) == NULL) {
        fprintf(stderr, "failed to allocate scene buffer\n");
        exit(EXIT_FAILURE);
    };

    sbo->capacity = INITAL_BUFFER_SIZE;
    return sbo;
}

void buffer_add(scene_buffer_t *scene_buffer, sphere_t sphere)
{
    if (scene_buffer->size == scene_buffer->capacity) {
        size_t new_size = (scene_buffer->capacity *= 2) * sizeof(sphere_t);

        if ((scene_buffer->buffer = realloc(scene_buffer->buffer, new_size)) == NULL) {
            fprintf(stderr, "failed to expand scene buffer\n");
            exit(EXIT_FAILURE);
        }
    }
    scene_buffer->buffer[scene_buffer->size++] = sphere;
}

size_t buffer_count_elements(scene_buffer_t *scene_buffer)
{
    return scene_buffer->size;
}

void *buffer_pointer(scene_buffer_t *scene_buffer, int *size)
{
    *size = sizeof(sphere_t) * scene_buffer->size;
    return (void *)scene_buffer->buffer;
}
