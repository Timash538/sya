#include <Window.hpp>
#include <stdexcept>

using namespace sya;

Window::Window(int width, int height, const std::string& title)
{
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

    if (!window)
    {
        throw std::runtime_error{"Failed to initialize Window " + title};
    }
}

Window::~Window()
{
    glfwDestroyWindow(window);
}

bool Window::shouldClose() const noexcept
{
    return glfwWindowShouldClose(window) != 0;
}