#include <GlfwRuntime.hpp>
#include <GLFW/glfw3.h>
#include <stdexcept>

using namespace sya;

GlfwRuntime::GlfwRuntime()
{
    if (!glfwInit())
    {
        throw std::runtime_error{"Failed to initialize GLFW"};
    }
}

GlfwRuntime::~GlfwRuntime()
{
    glfwTerminate();
}