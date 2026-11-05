#pragma once

#include <LogBuffer.hpp>
#include <LogRecord.hpp>

namespace sya::logging
{

class LogFormatter
{
  private:
    static void appendTwoDigits(LogBuffer& buffer, unsigned value) noexcept;

    static void appendThreeDigits(LogBuffer& buffer, unsigned value) noexcept;

  public:
    static void format(const LogRecord& record, LogBuffer& buffer) noexcept;
};
} // namespace sya