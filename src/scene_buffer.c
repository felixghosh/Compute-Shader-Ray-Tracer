#include "scene_buffer.h"

#include <stdio.h>
#include <stdlib.h>

#include "linalg.h"

/* Struct containing the objects making up the scene */
struct scene_t {
    /* Sphere buffer */
    int       sphere_capacity;
    int       sphere_size;
    sphere_t *sphere_buffer;

    /* Triangle buffer */
    int         triangle_capacity;
    int         triangle_size;
    triangle_t *triangle_buffer;
};

sphere_t create_sphere(float radius, vec3 center, vec3 color)
{
    return (sphere_t){
        (vec4){center.x, center.y, center.z, radius},
        (vec4){color.x,  color.y,  color.z,  0.0   },
    };
}

triangle_t create_triangle(vec3 points[3], vec3 color)
{
    return (triangle_t){
        (vec4){points[0].x, points[0].y, points[0].z, 0},
        (vec4){points[1].x, points[1].y, points[1].z, 0},
        (vec4){points[2].x, points[2].y, points[2].z, 0},
        (vec4){color.x,     color.y,     color.z,     0},
    };
}

scene_t *new_scene()
{
    scene_t *sbo;
    size_t   size = sizeof(scene_t);

    if ((sbo = calloc(1, size)) == NULL ||
        (sbo->sphere_buffer = calloc(sizeof(sphere_t), INITAL_BUFFER_SIZE)) == NULL ||
        (sbo->triangle_buffer = calloc(sizeof(triangle_t), INITAL_BUFFER_SIZE)) == NULL) {
        fprintf(stderr, "failed to allocate scene buffer\n");
        exit(EXIT_FAILURE);
    };

    sbo->sphere_capacity   = INITAL_BUFFER_SIZE;
    sbo->triangle_capacity = INITAL_BUFFER_SIZE;
    return sbo;
}

void scene_add_sphere(scene_t *scene, sphere_t sphere)
{
    if (scene->sphere_size == scene->sphere_capacity) {
        size_t new_size = (scene->sphere_capacity *= 2) * sizeof(sphere_t);

        if ((scene->sphere_buffer = realloc(scene->sphere_buffer, new_size)) == NULL) {
            fprintf(stderr, "failed to expand sphere buffer\n");
            exit(EXIT_FAILURE);
        }
    }
    scene->sphere_buffer[scene->sphere_size++] = sphere;
}

void scene_add_triangle(scene_t *scene, triangle_t triangle)
{
    if (scene->triangle_size == scene->triangle_capacity) {
        size_t new_size = (scene->triangle_capacity *= 2) * sizeof(triangle_t);

        if ((scene->triangle_buffer = realloc(scene->triangle_buffer, new_size)) == NULL) {
            fprintf(stderr, "failed to expand triangle buffer\n");
            exit(EXIT_FAILURE);
        }
    }
    scene->triangle_buffer[scene->triangle_size++] = triangle;
}

size_t scene_count_spheres(scene_t *scene)
{
    return scene->sphere_size;
}

size_t scene_count_triangles(scene_t *scene)
{
    return scene->triangle_size;
}

void *get_sphere_buffer(scene_t *scene, int *size)
{
    *size = sizeof(sphere_t) * scene->sphere_size;
    return (void *)scene->sphere_buffer;
}

void *get_triangle_buffer(scene_t *scene, int *size)
{
    *size = sizeof(triangle_t) * scene->triangle_size;
    return (void *)scene->triangle_buffer;
}
