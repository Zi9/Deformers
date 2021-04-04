#define WINDOW_TITLE "The Deformers"
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#include <GL/glew.h>
#include <GLFW/glfw3.h>

GLFWwindow* window;

int main(int argc, char** argv)
{
    if (!glfwInit())
        return -1;
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}