#ifndef LINALG_H
#define LINALG_H

/* Struct equivalent to OpenGL vec4 */
typedef struct vec4 {
    float x;
    float y;
    float z;
    float w;
} vec4;

/* Struct equivalent to OpenGL vec4 */
typedef struct vec3 {
    float x;
    float y;
    float z;
} vec3;

vec3 create_vec3(float x, float y, float z);
vec4 create_vec4(float x, float y, float z, float w);

#endif /* LINALG_H */
