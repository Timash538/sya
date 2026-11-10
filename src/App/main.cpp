#include <GLFW/glfw3.h>
#include <GlfwRuntime.hpp>
#include <LogMacros.hpp>
#include <Window.hpp>
#include <exception>

using namespace sya;

int main()
{
    try
    {

        LOGI("Sya started");
        {
            GlfwRuntime GRuntime;
            Window window(1280, 720, "Sya");

            while (!window.shouldClose())
            {
                glfwWaitEvents();
            }
        }
        LOGI("Sya stopped");
        return 0;
    }
    catch (const std::exception& error)
    {
        LOGE("Application failed: ", error.what());
        return 1;
    }
}