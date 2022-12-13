#ifndef SCENE_BUFFER_H
#define SCENE_BUFFER_H

#include <stddef.h>

#include "linalg.h"

#define INITIAL_BUFFER_SIZE 10

/* Object storing sphere properties */
typedef struct sphere_t {
    vec4  pos;    // xyz contains center and w radius
    vec4  color;  // alpha is unused
    float reflectivity;
    float refractivity;
    float __padding[2];  // Ensures vec4 alignment, simplifies the SSBO creation
} sphere_t;

/* Object storing triangle properties */
typedef struct triangle_t {
    vec4 p0;
    vec4 p1;
    vec4 p2;
    vec4 color; /* only xyz will be used, is kept vec4 to make alignment easier when sending it to
                   the gpu*/
} triangle_t;

/* Struct containing the object making up the scene */
typedef struct scene_t scene_t;

/* Triangle functions */
triangle_t create_triangle(vec3 points[3], vec3 color);

/* Sphere functions */
sphere_t create_sphere(float radius, vec3 center, vec3 color, float reflectivity,
                       float refractivity);

/* Scene buffer functions */
scene_t *new_scene();
void     scene_add_sphere(scene_t *scene, sphere_t sphere);
void     scene_add_triangle(scene_t *scene, triangle_t triangle);
size_t   scene_count_spheres(scene_t *scene);
size_t   scene_count_triangles(scene_t *scene);
void    *get_sphere_buffer(scene_t *scene, int *size);
void    *get_triangle_buffer(scene_t *scene, int *size);

#endif /* SCENE_BUFFER_H */
