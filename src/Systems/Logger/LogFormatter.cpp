#include <LogFormatter.hpp>

#include <chrono>

#include <SourceInfo.hpp>

using namespace sya::logging;

void LogFormatter::appendTwoDigits(LogBuffer& buffer, unsigned value) noexcept
{
    char digits[2] = {static_cast<char>('0' + value / 10), static_cast<char>('0' + value % 10)};

    buffer.append({digits, 2});
}

void LogFormatter::appendThreeDigits(LogBuffer& buffer, unsigned value) noexcept
{
    char digits[3] = {static_cast<char>('0' + value / 100),
                      static_cast<char>('0' + (value / 10) % 10),
                      static_cast<char>('0' + value % 10)};

    buffer.append({digits, 3});
}

void LogFormatter::format(const LogRecord& record, LogBuffer& buffer) noexcept
{
    using namespace std::chrono;

    // Время UTC
    const auto midnight = floor<days>(record.timestamp);

    const auto elapsed = floor<milliseconds>(record.timestamp - midnight);

    const hh_mm_ss time{elapsed};

    buffer.append("[");

    appendTwoDigits(buffer, static_cast<unsigned>(time.hours().count()));

    buffer.append(":");

    appendTwoDigits(buffer, static_cast<unsigned>(time.minutes().count()));

    buffer.append(":");

    appendTwoDigits(buffer, static_cast<unsigned>(time.seconds().count()));

    buffer.append(".");

    appendThreeDigits(buffer, static_cast<unsigned>(time.subseconds().count()));

    buffer.append(" UTC] ");

    // Класс / функция
    const auto source = SourceInfo::parse(record.location);

    if (!source.scope.empty())
    {
        buffer.append("[");
        buffer.append(source.scope);
        buffer.append("] ");
    }

    buffer.append("[");
    buffer.append(source.method);
    buffer.append("] ");

    // Уровень
    buffer.append("[");
    buffer.append(toString(record.level));
    buffer.append("] ");

    // Сообщение
    buffer.append(record.text());
}