#pragma once

#include <GLFW/glfw3.h>
#include <string>

namespace sya
{

class Window
{
public:
    Window(int width, int height, const std::string& title);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    bool shouldClose() const noexcept;
    GLFWwindow* nativeHandle() const noexcept;

private:
    GLFWwindow* window{nullptr};
};

} // namespace sya