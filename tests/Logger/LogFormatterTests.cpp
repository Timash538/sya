
#include <gtest/gtest.h>

#include <LogFormatter.hpp>

#include <string_view>
#include <chrono>
#include <source_location>

using namespace sya::logging;

TEST(LogFormatterTest, FormatsMessageWithEmptySourceLocation)
{
    LogRecord record{LogLevel::Info, std::source_location(), "Hello"};
    record.timestamp = std::chrono::system_clock::time_point{};
    char arr[512];
    LogBuffer buffer{arr};
    LogFormatter::format(record, buffer);
    EXPECT_EQ((std::string_view{arr, buffer.size()}), "[00:00:00.000 UTC] [] [INFO] Hello");
    EXPECT_FALSE(buffer.truncated());
}

namespace formatter_test
{
struct aStruct
{
    void aFunc(LogBuffer& buffer)
    {
        LogRecord record{LogLevel::Info, std::source_location::current(), "Hello"};
        record.timestamp = std::chrono::system_clock::time_point{};
        LogFormatter::format(record, buffer);
    }
};
} // namespace

TEST(LogFormatterTest, FormatsMessageWithSourceLocation)
{
    char arr[512];
    LogBuffer buffer{arr};
    formatter_test::aStruct s;
    s.aFunc(buffer);
    EXPECT_EQ((std::string_view{arr, buffer.size()}), "[00:00:00.000 UTC] [aStruct] [aFunc] [INFO] Hello");
    EXPECT_FALSE(buffer.truncated());
}

TEST(LogFormatterTest, FormatsMessageWithNonZeroTimestamp)
{
    using namespace std::chrono;
    LogRecord record{LogLevel::Info, std::source_location(), "Hello"};
    record.timestamp = system_clock::time_point{hours{12} + minutes{34} + seconds{56} + milliseconds{789}};
    char arr[512];
    LogBuffer buffer{arr};
    LogFormatter::format(record, buffer);
    EXPECT_EQ((std::string_view{arr, buffer.size()}), "[12:34:56.789 UTC] [] [INFO] Hello");
    EXPECT_FALSE(buffer.truncated());
}

TEST(LogFormatterTest, TruncatesWhenStorageIsTooSmall)
{
    LogRecord record{LogLevel::Info, std::source_location(), ""};
    record.timestamp = std::chrono::system_clock::time_point{};
    char arr[5];
    LogBuffer buffer{arr};
    LogFormatter::format(record, buffer);
    EXPECT_EQ((std::string_view{arr, buffer.size()}), "[00:0");
    EXPECT_TRUE(buffer.truncated());
}

