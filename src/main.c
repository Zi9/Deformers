#define WINDOW_TITLE "The Deformers"
#define WINDOW_WIDTH 960
#define WINDOW_HEIGHT 600

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdbool.h>
#include <stdio.h>

#include "t_map.h"

// TODO: Add error checking to this
GLuint compile_shader(const char* vertexShader, const char* fragmentShader)
{
    GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmetShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(vertexShaderID, 1, &vertexShader, NULL);
    glCompileShader(vertexShaderID);
    glShaderSource(fragmetShaderID, 1, &fragmentShader, NULL);
    glCompileShader(fragmetShaderID);
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, vertexShaderID);
    glAttachShader(ProgramID, fragmetShaderID);
    glLinkProgram(ProgramID);

    glDetachShader(ProgramID, vertexShaderID);
    glDetachShader(ProgramID, fragmetShaderID);

    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmetShaderID);
    return ProgramID;
}

GLFWwindow* window;
struct Map* current_map;

void onGameStart() { current_map = map_load(); }
void onGameStop() { map_unload(current_map); }
void onInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_CAPS_LOCK) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
void onUpdate() {}
void onRender() { map_render(current_map); }

int main(int argc, char** argv)
{
    if (!glfwInit())
        return -1;
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
    if (!window) {
        printf("Could not create window\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        printf("Could not initialize glew\n:");
        glfwTerminate();
        return -1;
    }
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    onGameStart();
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        onInput(window);

        onUpdate();

        glClear(GL_COLOR_BUFFER_BIT);
        onRender();
        glfwSwapBuffers(window);
    }
    onGameStop();
    glfwTerminate();
    return 0;
}