#pragma once

#include <fstream>
#include <string_view>
#include <string>
#include <cstddef>
#include <utility>

namespace sya::logging
{

class FileSink
{
  public:
    explicit FileSink(std::string path) : m_path(std::move(path)) {}
    bool write(std::string_view text);
    bool flush();

  private:
    static constexpr std::size_t FlushThreshold = 64 * 1024;

    std::ofstream m_file;
    std::string m_path;

    std::size_t m_bytesSinceFlush{0};
};
} // namespace sya::logging