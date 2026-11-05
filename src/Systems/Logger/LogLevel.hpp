#pragma once

#include <cstdint>
#include <string_view>

namespace sya::logging
{

enum class LogLevel : std::uint8_t
{
    Trace,
    Debug,
    Info,
    Warning,
    Error,
    Critical
};

constexpr std::string_view toString(LogLevel level) noexcept
{
    switch (level)
    {
    case LogLevel::Trace:
        return "TRACE";
    case LogLevel::Debug:
        return "DEBUG";
    case LogLevel::Info:
        return "INFO";
    case LogLevel::Warning:
        return "WARN";
    case LogLevel::Error:
        return "ERROR";
    case LogLevel::Critical:
        return "CRITICAL";
    }

    return "UNKNOWN";
}
} // namespace sya