#pragma once

#include <string_view>

namespace sya::logging
{

class ConsoleSink
{
  public:
    void write(std::string_view text) const noexcept;
};
} // namespace sya::logging