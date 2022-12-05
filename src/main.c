#include "gl_utils.h"
#include "scene_buffer.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

GLfloat timeValue = 0.0;

unsigned int shaderProgram;
const char  *shaderSource = "shaders/def.comp";

const unsigned int TEXTURE_WIDTH = 800, TEXTURE_HEIGHT = 800;

// Sets up shaders and textures buffer
GLuint init()
{
    shaderProgram = compile_shader(shaderSource, COMP);

    // Compute shader stuff
    GLuint tex_output;

    glGenTextures(1, &tex_output);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex_output);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, TEXTURE_WIDTH, TEXTURE_HEIGHT, 0, GL_RGBA, GL_FLOAT,
                 NULL);

    glBindImageTexture(0, tex_output, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

    int work_grp_cnt[3];

    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &work_grp_cnt[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &work_grp_cnt[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &work_grp_cnt[2]);

    printf("max global (total) work group counts x:%i y:%i z:%i\n", work_grp_cnt[0],
           work_grp_cnt[1], work_grp_cnt[2]);

    int work_grp_size[3];

    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &work_grp_size[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &work_grp_size[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &work_grp_size[2]);

    printf("max local (in one shader) work group sizes x:%i y:%i z:%i\n", work_grp_size[0],
           work_grp_size[1], work_grp_size[2]);

    int work_grp_inv;
    glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &work_grp_inv);
    printf("max local work group invocations %i\n", work_grp_inv);

    return tex_output;
}

int main(int argc, char *argv[])
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window =
        glfwCreateWindow(TEXTURE_WIDTH, TEXTURE_HEIGHT, "EDAN35 Project - Ray tracer", NULL, NULL);
    if (window == NULL) {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    glutInit(&argc, argv);
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        /* Problem: glewInit failed, something is seriously wrong. */
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    }
    fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

    GLuint texture = init();

    // Create frame buffer, link it to texture
    GLuint fboId = 0;
    glGenFramebuffers(1, &fboId);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fboId);
    glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    // keyboard state detection
    int prev_key_state = 0;
    int curr_key_state = 0;

    // Building our scene gemometry
    scene_buffer_t *scene_buff = new_buffer();
    buffer_add(scene_buff, (vec4){-4, -5, -10.0, 3});
    buffer_add(scene_buff, (vec4){3, 2, -10.0, 1});
    buffer_add(scene_buff, (vec4){0, 0, -10.0, 7});

    // Create scene ssbo
    int    scene_size;
    int    scene_n       = buffer_count_elements(scene_buff);
    void  *scene_pointer = buffer_pointer(scene_buff, &scene_size);
    GLuint scene_ssbo    = create_ssbo(scene_size, scene_pointer, GL_STATIC_DRAW, 2);

    // Scene light
    GLfloat light[] = {0.0, 1.0, 0.0};

    while (!glfwWindowShouldClose(window)) {

        // Passing data/uniforms
        glUniform3fv(glGetUniformLocation(shaderProgram, "light_position"), 1, &light);
        glUniform1i(glGetUniformLocation(shaderProgram, "n_spheres"), scene_n);

        // Shader computations
        glUseProgram(shaderProgram);
        glDispatchCompute((GLuint)TEXTURE_WIDTH, (GLuint)TEXTURE_HEIGHT, 1);

        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        // Copy texture
        glBlitFramebuffer(0, 0, TEXTURE_HEIGHT, TEXTURE_HEIGHT, 0, 0, TEXTURE_WIDTH, TEXTURE_HEIGHT,
                          GL_COLOR_BUFFER_BIT, GL_NEAREST);

        glfwPollEvents();
        if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE)) {
            glfwSetWindowShouldClose(window, 1);
        }

        // Keyboard click edge detector
        curr_key_state = glfwGetKey(window, GLFW_KEY_R);
        if (prev_key_state == 0 && curr_key_state == 1) {
            printf("Re-compiling shaders...\n");
            shaderProgram = compile_shader(shaderSource, COMP);
        }
        prev_key_state = curr_key_state;

        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
