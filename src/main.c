#include <GL/glew.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vector.h"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

#define WINDOW_NAME "bezier"

static void glfw_error_callback(int error, const char *description);
static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
static void framebuffer_size_callback(GLFWwindow *window, int width, int height);

#define return_defer(value) do { result = (value); goto defer; } while(0)

char *buffer_file(const char *file_path);
bool compile_shader(GLuint *shader, const char *path, GLenum type);

typedef struct {
    V2f pos;
} Vertex;

typedef enum {
    VA_POS = 0,
    VA_COUNT,
} Vertex_Attrib;

static inline Vertex vertex(V2f pos)
{
    Vertex v = {0};
    v.pos = pos;
    return v;
}

#define DA_INIT_CAP 256
#define da_append(da, item)                                                             \
    do {                                                                                \
        if((da)->count >= (da)->capacity) {                                             \
            (da)->capacity = (da)->capacity == 0 ? DA_INIT_CAP : (da)->capacity * 2;    \
            (da)->items = realloc((da)->items, (da)->capacity * sizeof(*(da)->items));  \
            assert((da)->items != NULL && "Buy more RAM lol");                          \
        }                                                                               \
                                                                                        \
        (da)->items[(da)->count++] = (item);                                            \
    } while(0)

typedef struct {
    Vertex *items;
    size_t count;
    size_t capacity;
} Vertex_List;

void create_spline(Vertex_List *vertices, V2f a, V2f b, V2f c, V2f d, unsigned int resolution)
{
    for(float t = 0.0f; t < 1.0f; t += 1.0f/(float)resolution) {
        da_append(vertices, vertex(v2f_spline(a, b, c, d, t)));
    }
}

int main(void)
{
    int result = 0;
    GLFWwindow *window = NULL;

    char *vert_shader_source = NULL;
    char *frag_shader_source = NULL;

    glfwSetErrorCallback(glfw_error_callback);

    if(!glfwInit()) return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);  // OpenGL minimum version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME, NULL, NULL);
    if(!window) return_defer(-1);

    glfwMakeContextCurrent(window);

    // GLFW Context ready; initialize OpenGL
    GLenum err = glewInit();
    if(err != GLEW_OK) {
        fprintf(stderr, "GLEW ERROR: %s\n", glewGetErrorString(err));
    }

    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glfwSwapInterval(1);

    Vertex_List vertices = {0};
    /* create_spline(&vertices, v2f(-1.0f, -0.5f), v2f(-0.5f, 0.5f), */
    /*                          v2f(0.5f, 0.5f), v2f(1.0f, -0.5f), 4); */
    V2f a = v2f_scale(v2f(-1.0f, -0.5f), 0.5f);
    V2f b = v2f_scale(v2f(-0.5f, 0.5f), 0.5f);
    V2f c = v2f_scale(v2f(0.5f, 0.5f), 0.5f);
    V2f d = v2f_scale(v2f(1.0f, -0.5f), 0.5f);
    /* da_append(&vertices, vertex(a)); */
    /* da_append(&vertices, vertex(b)); */
    /* da_append(&vertices, vertex(c)); */
    /* da_append(&vertices, vertex(d)); */

    for(unsigned int t = 0; t <= 100; t += 5) {
        da_append(&vertices, vertex(v2f_spline(a, b, c, d, (float)t/100.0f)));
    }

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(*vertices.items)*vertices.count, vertices.items, GL_STATIC_DRAW);

    glVertexAttribPointer(VA_POS, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, pos));
    glEnableVertexAttribArray(VA_POS);

    /* glVertexAttribPointer(VA_COLOR, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, color)); */
    /* glEnableVertexAttribArray(VA_COLOR); */

    GLuint vertex_shader;
    if(!compile_shader(&vertex_shader, "shaders/main.vert", GL_VERTEX_SHADER)) return_defer(-2);

    GLuint fragment_shader;
    if(!compile_shader(&fragment_shader, "shaders/main.frag", GL_FRAGMENT_SHADER)) return_defer(-2);

    GLuint shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);

    glBindFragDataLocation(shader_program, 0, "out_color");
    glLinkProgram(shader_program);

    glUseProgram(shader_program);

    /* double current_time = 0.0f; */
    /* double last_time = 0.0f; */
    /* double delta_time = 0.0f; */

    while(!glfwWindowShouldClose(window)) {
        /* current_time = glfwGetTime(); */
        /* delta_time = current_time - last_time; */
        /* last_time = current_time; */

        glClear(GL_COLOR_BUFFER_BIT);
        glDrawArrays(GL_LINE_STRIP, 0, vertices.count);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

defer:

    if(vert_shader_source) free(vert_shader_source);
    if(frag_shader_source) free(frag_shader_source);

    if(window) glfwDestroyWindow(window);
    glfwTerminate();

    return result;
}

static void glfw_error_callback(int error, const char *description)
{
    (void) error;
    fprintf(stderr, "GLFW ERROR: %s\n", description);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    (void) scancode;
    (void) mods;
    switch(key) {
        case GLFW_KEY_ESCAPE: {
            if(action == GLFW_PRESS) glfwSetWindowShouldClose(window, GLFW_TRUE);
        } break;
        default: break;
    }
}

static void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    (void) window;
    glViewport(0, 0, width, height);
}

char *buffer_file(const char *file_path)
{
    char *buf = NULL;
    FILE *fp = NULL;

    fp = fopen(file_path, "r");
    if(fp == NULL) goto defer;
    if(fseek(fp, 0L, SEEK_END) < 0) goto defer;

    long size = ftell(fp);
    if(size < 0) goto defer;

    buf = malloc(size + 1);
    if(buf == NULL) goto defer;

    if(fseek(fp, 0, SEEK_SET) < 0) goto defer;

    fread(buf, 1, size, fp);
    if(ferror(fp)) goto defer;

    buf[size] = '\0';

    if(fp) {
        fclose(fp);
        errno = 0;
    }

    return buf;

defer:

    if(fp) {
        int s = errno;
        fclose(fp);
        errno = s;
    }

    if(buf) {
        free(buf);
    }

    return NULL;
}


bool compile_shader(GLuint *shader, const char *path, GLenum type)
{
    GLchar *source = buffer_file(path);
    if(!source) {
        fprintf(stderr, "failed to read `%s`: %s\n", path, strerror(errno));
        return false;
    }

    *shader = glCreateShader(type);
    glShaderSource(*shader, 1, (const GLchar * const *)&source, NULL);
    glCompileShader(*shader);

    free(source);

    GLint status;
    glGetShaderiv(*shader, GL_COMPILE_STATUS, &status);

    if(status != GL_TRUE) {
        char buffer[512];
        glGetShaderInfoLog(*shader, 512, NULL, buffer);
        fprintf(stderr, "failed to compile shader `%s`:\n\t%s", path, buffer);

        return false;
    }

    return true;
}
