#pragma once

namespace sya
{

class GlfwRuntime
{
  public:
    GlfwRuntime();
    ~GlfwRuntime();

    GlfwRuntime(const GlfwRuntime&) = delete;
    GlfwRuntime& operator=(const GlfwRuntime&) = delete;
};

} // namespace sya