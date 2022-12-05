#include "gl_utils.h"

#include <GL/glew.h>
#include <GL/glut.h>
#include <stdio.h>
#include <string.h>

unsigned int load_shader(const char *filepath, shader_type type)
{
    FILE *fp = fopen(filepath, "r");
    if (fp == NULL) {
        printf("Error! Shader source file can't be opened!\n");
        return -1;
    }

    fseek(fp, 0, SEEK_END);
    int fileSize = ftell(fp);
    rewind(fp);

    char *shaderSource = malloc(sizeof(char) * fileSize + 1);
    memset(shaderSource, 0, fileSize + 1);
    int n = fread(shaderSource, sizeof(char), fileSize, fp);
    fclose(fp);

    unsigned int shader;
    switch (type) {
    case VERT:
        shader = glCreateShader(GL_VERTEX_SHADER);
        break;
    case FRAG:
        shader = glCreateShader(GL_FRAGMENT_SHADER);
        break;
    default:
        shader = glCreateShader(GL_COMPUTE_SHADER);
        break;
    }

    glShaderSource(shader, 1, (const char **)&shaderSource, NULL);
    glCompileShader(shader);

    int  success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        printf("Error! Compilation of %d shader failed: %s\n", type, infoLog);
    }
    free(shaderSource);

    return shader;
}

unsigned int compile_shader(const char *filepath, shader_type type)
{
    unsigned int shader  = load_shader(filepath, type);
    unsigned int program = glCreateProgram();
    glAttachShader(program, shader);
    glLinkProgram(program);

    int  success;
    char infoLog[512];

    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        printf("Error! Program linking failed: %s\n", infoLog);
    }

    glUseProgram(program);
    glDeleteShader(shader);
    return program;
}

GLuint create_ssbo(GLsizeiptr size, const void *data, GLenum usage, GLuint index)
{
    GLuint ssbo;
    glGenBuffers(1, &ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, usage);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    return ssbo;
}
