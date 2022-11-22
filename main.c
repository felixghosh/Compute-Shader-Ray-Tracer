
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

GLfloat timeValue = 0.0;
unsigned int shaderProgram;

 const unsigned int TEXTURE_WIDTH = 512, TEXTURE_HEIGHT = 512;

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
    else if(strcmp(type, "compute") == 0)
        shader = glCreateShader(GL_COMPUTE_SHADER);

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
    unsigned int computeShader = load_shader("def.comp", "compute");

    shaderProgram = glCreateProgram();
    //glAttachShader(shaderProgram, vertexShader);
    //glAttachShader(shaderProgram, fragmentShader);
    glAttachShader(shaderProgram, computeShader);
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
    glDeleteShader(computeShader);

    glGenVertexArrays(NumVAOs, VAOs);
    glBindVertexArray(VAOs[Triangles]);
    glBindBuffer(GL_ARRAY_BUFFER, buffers[ArrayBuffer]);
    glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glVertexAttribPointer(vColor, 3, GL_FLOAT, GL_FALSE, 0, (void*)sizeof(positions));
    glEnableVertexAttribArray(vPosition);
    glEnableVertexAttribArray(vColor);

    //Compute shader stuff
    GLuint tex_output;

    glGenTextures(1, &tex_output);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex_output);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);

    glBindImageTexture(0, tex_output, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

    int work_grp_cnt[3];

    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &work_grp_cnt[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &work_grp_cnt[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &work_grp_cnt[2]);

    printf("max global (total) work group counts x:%i y:%i z:%i\n",
    work_grp_cnt[0], work_grp_cnt[1], work_grp_cnt[2]);

    int work_grp_size[3];

    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &work_grp_size[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &work_grp_size[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &work_grp_size[2]);

    printf("max local (in one shader) work group sizes x:%i y:%i z:%i\n",
    work_grp_size[0], work_grp_size[1], work_grp_size[2]);

    int work_grp_inv;
    glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &work_grp_inv);
    printf("max local work group invocations %i\n", work_grp_inv);
}

void display(){
    /*static const float black[] = {0.0f, 0.0f, 0.0f, 0.0f};
    glClearBufferfv(GL_COLOR, 0, black);

    glBindVertexArray(VAOs[Triangles]);
    glDrawArrays(GL_TRIANGLES, 0, NumVertices);*/
}

int main(int argc, char* argv[])
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
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
        //timeValue += 0.01;
        //GLint timeLoc = glGetUniformLocation(shaderProgram, "time");
        //glUniform1f(timeLoc, timeValue);
        display();
        glfwSwapBuffers(window);
        glfwPollEvents();
        
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    return 0;
}
