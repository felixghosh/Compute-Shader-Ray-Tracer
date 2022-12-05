#ifndef GL_UTILS_H
#define GL_UTILS_H

#include <GL/glew.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h>

typedef enum shader_type { VERT, FRAG, COMP } shader_type;

unsigned int load_shader(const char *filepath, shader_type type);
unsigned int compile_shader(const char *filepath, shader_type type);
GLuint       create_ssbo(GLsizeiptr size, const void *data, GLenum usage, GLuint index);

#endif /* GL_UTILS_H */