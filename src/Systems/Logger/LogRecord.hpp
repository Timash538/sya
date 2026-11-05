#pragma once

#include <array>
#include <chrono>
#include <source_location>
#include <string_view>

#include <LogBuffer.hpp>
#include <LogLevel.hpp>

namespace sya::logging
{

struct LogRecord
{
    static constexpr std::size_t MessageCapacity = 512;
    template <typename... Args>
    LogRecord(LogLevel level, std::source_location location, std::string_view title, Args&&... args) noexcept
        : level(level), timestamp(std::chrono::system_clock::now()), location(location)
    {
        LogBuffer logBuffer(message);
        logBuffer.append(title);
        ((logBuffer.append(" | "), logBuffer.appendValue(args)), ...);
        messageSize = logBuffer.size();
        truncated = logBuffer.truncated();
    }

    std::string_view text() const noexcept
    {
        return {message.data(), messageSize};
    }
    LogLevel level;

    std::chrono::system_clock::time_point timestamp;

    std::source_location location;

    std::array<char, MessageCapacity> message{};
    std::size_t messageSize{0};
    bool truncated{false};
};
} // namespace sya::logging