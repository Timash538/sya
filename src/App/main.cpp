#include <GLFW/glfw3.h>
#include <LogMacros.hpp>

int main()
{
    LOGI("Sya started");

    if (!glfwInit())
    {
        LOGE("Failed to initialize GLFW");
        return 1;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Sya", nullptr, nullptr);

    if (!window)
    {
        LOGE("Failed to create window");
        glfwTerminate();
        return 1;
    }

    while (!glfwWindowShouldClose(window))
    {
        glfwWaitEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    LOGI("Sya stopped");
    return 0;
}