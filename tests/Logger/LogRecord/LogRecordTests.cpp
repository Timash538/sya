
#include <gtest/gtest.h>

#include <LogRecord.hpp>

#include <source_location>
#include <string>
#include <string_view>
#include <vector>

using namespace sya::logging;

TEST(LogRecordTest, NormalMessage)
{
    LogRecord msg(LogLevel::Info, std::source_location::current(), "Hello");

    EXPECT_EQ(msg.messageSize, 5);
    EXPECT_EQ(msg.text(), "Hello");
    EXPECT_FALSE(msg.truncated);
    EXPECT_EQ(msg.level, LogLevel::Info);
}

TEST(LogRecordTest, EmptyMessage)
{
    LogRecord msg(LogLevel::Info, std::source_location::current(), "");

    EXPECT_EQ(msg.messageSize, 0);
    EXPECT_TRUE(msg.text().empty());
    EXPECT_FALSE(msg.truncated);
}

TEST(LogRecordTest, TruncatesLongMessage)
{
    std::string text(600, 'a');

    LogRecord msg(LogLevel::Info, std::source_location::current(), text);

    EXPECT_EQ(msg.messageSize, LogRecord::MessageCapacity);
    EXPECT_EQ(msg.text(), std::string_view(text.data(), 512));
    EXPECT_TRUE(msg.truncated);
}

TEST(LogRecordTest, ExactCapacity)
{
    std::string text(512, 'x');

    LogRecord msg(LogLevel::Info, std::source_location::current(), text);

    EXPECT_EQ(msg.messageSize, 512);
    EXPECT_EQ(msg.text(), text);
    EXPECT_FALSE(msg.truncated);
}

TEST(LogRecordTest, FormatsMultipleArguments)
{
    LogRecord msg(LogLevel::Debug, std::source_location::current(), "Entity spawned", 42, true, 3.5f);

    EXPECT_EQ(msg.text(), "Entity spawned | 42 | true | 3.5");
    EXPECT_FALSE(msg.truncated);
}

TEST(LogRecordTest, FormatsContainers)
{
    std::vector<int> values{1, 2, 3};

    LogRecord msg(LogLevel::Info, std::source_location::current(), "Values", values);

    EXPECT_EQ(msg.text(), "Values | [1, 2, 3]");
    EXPECT_FALSE(msg.truncated);
}

TEST(LogRecordTest, TruncatesArguments)
{
    std::string text(600, 'z');

    LogRecord msg(LogLevel::Warning, std::source_location::current(), "Payload", text);

    EXPECT_EQ(msg.messageSize, 512);
    EXPECT_TRUE(msg.truncated);
    EXPECT_TRUE(msg.text().starts_with("Payload | "));
}

TEST(LogRecordTest, StoresSourceLocation)
{
    const auto location = std::source_location::current();

    LogRecord msg(LogLevel::Info, location, "Test");

    EXPECT_EQ(msg.location.line(), location.line());
    EXPECT_STREQ(msg.location.file_name(), location.file_name());
    EXPECT_STREQ(msg.location.function_name(), location.function_name());
}

TEST(LogRecordTest, PreservesLogLevel)
{
    LogRecord msg(LogLevel::Critical, std::source_location::current(), "Fatal error");

    EXPECT_EQ(msg.level, LogLevel::Critical);
}

TEST(LogRecordTest, MessageBufferNotNullTerminatedRequired)
{
    std::string text(512, 'a');

    LogRecord msg(LogLevel::Info, std::source_location::current(), text);

    EXPECT_EQ(msg.text().size(), 512);
    EXPECT_EQ(msg.text().back(), 'a');
}
