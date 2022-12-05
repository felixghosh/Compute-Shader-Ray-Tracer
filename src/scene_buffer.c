#include "scene_buffer.h"

#include <stdio.h>
#include <stdlib.h>

/* Struct containing the object making up the scene */
struct scene_buffer_t {
    int   capacity;
    int   size;
    vec4 *buffer;
};

scene_buffer_t *new_buffer()
{
    scene_buffer_t *sbo;
    size_t          size = sizeof(scene_buffer_t);

    if ((sbo = calloc(1, size)) == NULL ||
        (sbo->buffer = calloc(sizeof(vec4), INITAL_BUFFER_SIZE)) == NULL) {
        fprintf(stderr, "failed to allocate scene buffer\n");
        exit(EXIT_FAILURE);
    };

    sbo->capacity = INITAL_BUFFER_SIZE;
    return sbo;
}

void buffer_add(scene_buffer_t *scene_buffer, vec4 obj)
{
    if (scene_buffer->size == scene_buffer->capacity) {
        size_t new_size = (scene_buffer->capacity *= 2) * sizeof(vec4);

        if ((scene_buffer->buffer = realloc(scene_buffer->buffer, new_size)) == NULL) {
            fprintf(stderr, "failed to expand scene buffer\n");
            exit(EXIT_FAILURE);
        }
    }
    scene_buffer->buffer[scene_buffer->size++] = obj;
}

size_t buffer_count_elements(scene_buffer_t *scene_buffer)
{
    return scene_buffer->size;
}

void *buffer_pointer(scene_buffer_t *scene_buffer, int *size)
{
    *size = sizeof(vec4) * scene_buffer->size;
    return (void *)scene_buffer->buffer;
}
