#include <ConsoleSink.hpp>

#include <string_view>
#include <cstdio>

using namespace sya::logging;

 void ConsoleSink::write(std::string_view text) const noexcept
{
    std::fwrite(text.data(), 1, text.size(), stdout);

    std::fputc('\n', stdout);
    std::fflush(stdout);
}