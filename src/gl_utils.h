#ifndef GL_UTILS_H
#define GL_UTILS_H


typedef enum shader_type {VERT, FRAG, COMP} shader_type;

unsigned int load_shader(char *filepath, shader_type type);


#endif /* GL_UTILS_H */