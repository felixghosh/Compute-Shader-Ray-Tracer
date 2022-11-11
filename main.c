
#include <GL/glew.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define NumVAOs 1
#define NumBuffers 1

const int ArrayBuffer = 0;
const int Triangles = 0;
enum Attrib_IDs { vPosition = 0, vColor = 1};

GLuint VAOs[NumVAOs];
GLuint buffers[NumBuffers];

const GLuint NumVertices = 6;

unsigned int load_shader(char* filepath, char* type){
    FILE* fp = fopen(filepath, "r");
    if(fp == NULL){
        printf("Error! Shader source file can't be opened!\n");
        return -1;
    }

    fseek(fp, 0, SEEK_END );
    int fileSize = ftell(fp);
    rewind(fp);

    char* shaderSource = malloc(sizeof(char)*fileSize+1);
    memset(shaderSource, 0, fileSize+1);
    int n = fread(shaderSource, sizeof(char), fileSize, fp);
    fclose(fp);

    unsigned int shader;
    if(strcmp(type, "vertex") == 0)
        shader = glCreateShader(GL_VERTEX_SHADER);
    else if(strcmp(type, "fragment") == 0)
        shader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(shader, 1, (const char**)&shaderSource, NULL);
    glCompileShader(shader);

    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if(!success){
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        printf("Error! Compilation of %s shader failed: %s\n", type, infoLog);
    }
    free(shaderSource);

    return shader;
}

void init(){
    static const GLfloat positions[6][3] = {
        {-0.90, -0.90, 0.0},
        {0.85, -0.90, 0.0},
        {-0.90, 0.85, 0.0},
        {0.90, -0.85, 0.0},
        {0.90, 0.90, 0.0},
        {-0.85, 0.90, 0.0}
    };

    static const GLfloat colors[6][3] = {
        {1.0, 0.0, 0.0},
        {1.0, 1.0, 0.0},
        {1.0, 1.0, 1.0},
        {0.0, 1.0, 0.0},
        {0.0, 0.0, 1.0},
        {0.0, 1.0, 1.0},
    };

    size_t buffer_size = sizeof positions + sizeof colors;

    glCreateBuffers(NumBuffers, buffers);
    glNamedBufferStorage(buffers[ArrayBuffer], buffer_size, NULL, GL_DYNAMIC_STORAGE_BIT);
    glNamedBufferSubData(buffers[ArrayBuffer], 0, sizeof positions, positions);
    glNamedBufferSubData(buffers[ArrayBuffer], sizeof positions, sizeof colors, colors);

    unsigned int vertexShader = load_shader("def.vert", "vertex");
    unsigned int fragmentShader = load_shader("def.frag", "fragment");

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    int success;
    char infoLog[512];

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        printf("Error! Program linking failed: %s\n", infoLog);
    }

    glUseProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glGenVertexArrays(NumVAOs, VAOs);
    glBindVertexArray(VAOs[Triangles]);
    glBindBuffer(GL_ARRAY_BUFFER, buffers[ArrayBuffer]);
    glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0, (void*)sizeof(positions));
    glEnableVertexAttribArray(vPosition);
    glEnableVertexAttribArray(vColor);
}

void display(){
    static const float black[] = {0.0f, 0.0f, 0.0f, 0.0f};
    glClearBufferfv(GL_COLOR, 0, black);

    glBindVertexArray(VAOs[Triangles]);
    glDrawArrays(GL_TRIANGLES, 0, NumVertices);
}

int main(int argc, char* argv[])
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glutInit(&argc, argv);
    GLenum err = glewInit();
    if (GLEW_OK != err){
        /* Problem: glewInit failed, something is seriously wrong. */
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    }
    fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

    init();

    while(!glfwWindowShouldClose(window))
    {
        display();
        glfwSwapBuffers(window);
        glfwPollEvents();    
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    return 0;
}
