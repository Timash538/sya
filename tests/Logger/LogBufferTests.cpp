
#include <gtest/gtest.h>

#include <LogBuffer.hpp>

#include <string_view>
#include <span>


using namespace sya::logging;

TEST(LogBufferTest, EmptyStorage)
{
    LogBuffer buffer{std::span<char>{}};

    EXPECT_EQ(buffer.size(), 0);
    EXPECT_EQ(buffer.remaining(), 0);
    EXPECT_FALSE(buffer.truncated());

    buffer.append("Hello");

    EXPECT_EQ(buffer.size(), 0);
    EXPECT_TRUE(buffer.truncated());
}

TEST(LogBufferTest, ExactFit)
{
    char arr[3];
    LogBuffer buffer{arr};
    buffer.append("abc");
    EXPECT_EQ(buffer.size(), 3);
    EXPECT_EQ(buffer.remaining(), 0);
    EXPECT_FALSE(buffer.truncated());
    EXPECT_EQ((std::string_view{arr, buffer.size()}), "abc");
}

TEST(LogBufferTest, Overflow)
{
    char arr[3];
    LogBuffer buffer{arr};
    buffer.append("abcd");
    EXPECT_EQ(buffer.size(), 3);
    EXPECT_EQ(buffer.remaining(), 0);
    EXPECT_TRUE(buffer.truncated());
    EXPECT_EQ((std::string_view{arr, buffer.size()}), "abc");
}

TEST(LogBufferTest, AppendMultipleParts)
{
    char arr[6];
    LogBuffer buffer{arr};
    buffer.append("abc");
    buffer.append("def");
    EXPECT_EQ(buffer.size(), 6);
    EXPECT_EQ(buffer.remaining(), 0);
    EXPECT_EQ((std::string_view{arr, buffer.size()}), "abcdef");
    EXPECT_FALSE(buffer.truncated());
}