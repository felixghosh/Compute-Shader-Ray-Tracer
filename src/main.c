#include <math.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "gl_utils.h"
#include "linalg.h"
#include "scene_buffer.h"

#define TIME_CONST (50)

GLfloat timeValue    = 0.0;
float   elapsed_time = 0.0;

GLfloat camera_pos[] = {0.0, 2.0, 1.0};
GLfloat camera_angle = 0.0;

struct timespec t0, t1;

float speed = 0.3;

unsigned int shaderProgram;
const char  *shaderSource = "shaders/def.comp";

const unsigned int TEXTURE_WIDTH = 800, TEXTURE_HEIGHT = 800;

void update_time()
{
    clock_gettime(CLOCK_REALTIME, &t1);
    elapsed_time = (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / 1000000000.0;
    clock_gettime(CLOCK_REALTIME, &t0);
}

void movCamera(float distX, float distY, float distZ)
{
    camera_pos[0] +=
        (float)sin(-camera_angle) * distZ*elapsed_time*TIME_CONST + (float)sin(-camera_angle + M_PI / 2) * distX*elapsed_time*TIME_CONST;
    camera_pos[1] += distY*elapsed_time*TIME_CONST;
    camera_pos[2] +=
        (float)cos(-camera_angle) * distZ*elapsed_time*TIME_CONST + (float)cos(-camera_angle + M_PI / 2) * distX*elapsed_time*TIME_CONST;
}

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
    vec3 vertices[] = {
        create_vec3(-20.0f, 0.0f, 50.0f),   create_vec3(20.0f, 0.0f, 50.0f),
        create_vec3(20.0f, 0.0f, -50.0f),  // Floor 1
        create_vec3(-20.0f, 0.0f, 50.0f),   create_vec3(20.0f, 0.0f, -50.0f),
        create_vec3(-20.0f, 0.0f, -50.0f),  // Floor 2
        create_vec3(-20.0f, 0.0f, -50.0f),  create_vec3(20.0f, 0.0f, -50.0f),
        create_vec3(20.0f, 40.0f, -50.0f),  // Back wall 1
        create_vec3(-20.0f, 0.0f, -50.0f),  create_vec3(20.0f, 40.0f, -50.0f),
        create_vec3(-20.0f, 40.0f, -50.0f),  // Back wall 2
        create_vec3(-20.0f, 40.0f, 50.0f),  create_vec3(-20.0f, 40.0f, -50.0f),
        create_vec3(20.0f, 40.0f, 50.0f),  // Ceiling 1
        create_vec3(20.0f, 40.0f, 50.0f),   create_vec3(-20.0f, 40.0f, -50.0f),
        create_vec3(20.0f, 40.0f, -50.0f),  // Ceiling 2
        create_vec3(-20.0f, 0.0f, 50.0f),   create_vec3(-20.0f, 40.0f, -50.0f),
        create_vec3(-20.0f, 40.0f, 50.0f),  // Red wall 1
        create_vec3(-20.0f, 0.0f, 50.0f),   create_vec3(-20.0f, 0.0f, -50.0f),
        create_vec3(-20.0f, 40.0f, -50.0f),  // Red wall 2
        create_vec3(20.0f, 40.0f, 50.0f),    create_vec3(20.0f, 40.0f, -50.0f),
        create_vec3(20.0f, 0.0f, 50.0f),  // Green wall 1
        create_vec3(20.0f, 40.0f, -50.0f),    create_vec3(20.0f, 0.0f, -50.0f),
        create_vec3(20.0f, 0.0f, 50.0f)  // Green wall 2
    };

    scene_t *scene = new_scene();
    scene_add_triangle(scene,
                       create_triangle(&vertices[0], create_vec3(0.9f, 0.9f, 0.9f)));  // Floor 1
    scene_add_triangle(scene,
                       create_triangle(&vertices[3], create_vec3(0.9f, 0.9f, 0.9f)));  // Floor 2
    scene_add_triangle(
        scene, create_triangle(&vertices[6], create_vec3(0.9f, 0.9f, 0.9f)));  // Back wall 1
    scene_add_triangle(
        scene, create_triangle(&vertices[9], create_vec3(0.9f, 0.9f, 0.9f)));  // Back wall 2
    scene_add_triangle(scene,
                       create_triangle(&vertices[12], create_vec3(0.9f, 0.9f, 0.9f)));  // Ceiling 1
    scene_add_triangle(scene,
                       create_triangle(&vertices[15], create_vec3(0.9f, 0.9f, 0.9f)));  // Ceiling 2
    scene_add_triangle(
        scene, create_triangle(&vertices[18], create_vec3(1.0f, 0.1f, 0.1f)));  // Red wall 1
    scene_add_triangle(
        scene, create_triangle(&vertices[21], create_vec3(1.0f, 0.1f, 0.1f)));  // Red wall 2
    scene_add_triangle(
        scene, create_triangle(&vertices[24], create_vec3(0.1f, 0.6f, 0.1f)));  // Green wall 1
    scene_add_triangle(
        scene, create_triangle(&vertices[27], create_vec3(0.1f, 0.6f, 0.1f)));  // Green wall 2

    scene_add_sphere(scene,
                     create_sphere(3, create_vec3(-7, 3, -20.0), create_vec3(0, 1, 0), 0, 0));
    scene_add_sphere(scene, create_sphere(3, create_vec3(0, 3, -20.0), create_vec3(0, 0, 1), 0, 0));
    scene_add_sphere(scene, create_sphere(3, create_vec3(7, 3, -20.0), create_vec3(1, 0, 0), 0, 0));

    scene_add_sphere(scene, create_sphere(3, create_vec3(9.0f, 10.0f, 0.0f),
                                          create_vec3(1.0f, 0.6f, 0.1f), 0.2, 0));
    scene_add_sphere(scene, create_sphere(3, create_vec3(0.0f, 13.0f, 0.0f),
                                          create_vec3(0.1f, 0.1f, 1.0f), 0.9, 0));

    // Create sphere ssbo:s
    int    sphere_size;
    int    sphere_n    = scene_count_spheres(scene);
    void  *sphere_ptr  = get_sphere_buffer(scene, &sphere_size);
    GLuint sphere_ssbo = create_ssbo(sphere_size, sphere_ptr, GL_STATIC_DRAW, 2);

    // Create triangle ssbo:s
    int    triangle_size;
    int    triangle_n    = scene_count_triangles(scene);
    void  *triangle_ptr  = get_triangle_buffer(scene, &triangle_size);
    GLuint triangle_ssbo = create_ssbo(triangle_size, triangle_ptr, GL_STATIC_DRAW, 3);

    printf("N triangles: %d\n", triangle_n);

    // Scene light
    GLfloat light[] = {0.0, 30, -5};

    while (!glfwWindowShouldClose(window)) {
        update_time();
        printf("fps: %5u\n", (int)(1 / elapsed_time));

        // Passing uniforms
        glUniform3fv(glGetUniformLocation(shaderProgram, "light_position"), 1, &light);
        glUniform1i(glGetUniformLocation(shaderProgram, "n_spheres"), sphere_n);
        glUniform1i(glGetUniformLocation(shaderProgram, "n_triangles"), triangle_n);
        glUniform3fv(glGetUniformLocation(shaderProgram, "camera_pos"), 1, &camera_pos);
        glUniform1f(glGetUniformLocation(shaderProgram, "camera_angle"), camera_angle);

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

        if (glfwGetKey(window, GLFW_KEY_S) == 1) {
            // camera_pos[2] += 0.1;
            movCamera(0.0, 0.0, speed);
        }
        if (glfwGetKey(window, GLFW_KEY_W) == 1) {
            // camera_pos[2] -= 0.1;
            movCamera(0.0, 0.0, -speed);
        }
        if (glfwGetKey(window, GLFW_KEY_D) == 1) {
            // camera_pos[0] += 0.1;
            movCamera(speed, 0.0, 0.0);
        }
        if (glfwGetKey(window, GLFW_KEY_A) == 1) {
            // camera_pos[0] -= 0.1;
            movCamera(-speed, 0.0, 0.0);
        }
        if (glfwGetKey(window, GLFW_KEY_T) == 1) {
            camera_pos[1] += speed*elapsed_time*TIME_CONST;
        }
        if (glfwGetKey(window, GLFW_KEY_G) == 1) {
            camera_pos[1] -= speed*elapsed_time*TIME_CONST;
        }
        if (glfwGetKey(window, GLFW_KEY_Q) == 1) {
            camera_angle -= 0.01*elapsed_time*TIME_CONST;
        }
        if (glfwGetKey(window, GLFW_KEY_E) == 1) {
            camera_angle += 0.01*elapsed_time*TIME_CONST;
        }
        if (glfwGetKey(window, GLFW_KEY_Y) == 1) {
            light[2] -= 0.1;
        }
        if (glfwGetKey(window, GLFW_KEY_H) == 1) {
            light[2] += 0.1;
        }

        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
