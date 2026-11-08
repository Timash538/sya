#include <gtest/gtest.h>

#include <FileSink.hpp>

#include <filesystem>
#include <stdexcept>
#include <system_error>
#include <utility>
#include <random>
#include <fstream>
#include <string>

using namespace sya::logging;

class TemporaryDirectory
{
  public:
    TemporaryDirectory()
    {
        const auto basePath = std::filesystem::temp_directory_path();
        std::random_device random;

        for (int attempt = 0; attempt < 100; ++attempt)
        {
            auto candidate = basePath / ("sya-test-" + std::to_string(random()));

            if (std::filesystem::create_directory(candidate))
            {
                m_path = std::move(candidate);
                return;
            }
        }

        throw std::runtime_error{"Could not create a unique temporary directory"};
    }

    ~TemporaryDirectory()
    {
        std::error_code error;
        std::filesystem::remove_all(m_path, error);
    }

    TemporaryDirectory(const TemporaryDirectory&) = delete;
    TemporaryDirectory& operator=(const TemporaryDirectory&) = delete;

    const std::filesystem::path& path() const noexcept
    {
        return m_path;
    }

  private:
    std::filesystem::path m_path;
};

TEST(FileSinkTest, WritesMessage)
{
    TemporaryDirectory dir;
    const auto filePath = dir.path() / "test.log";

    FileSink sink{filePath.string()};
    ASSERT_TRUE(sink.write("Hello"));
    ASSERT_TRUE(sink.flush());

    std::ifstream file{filePath};
    ASSERT_TRUE(file.is_open());

    std::string line;
    ASSERT_TRUE(std::getline(file, line));
    EXPECT_EQ(line, "Hello");
}

TEST(FileSinkTest, Append)
{
    TemporaryDirectory dir;
    const auto filePath = dir.path() / "test.log";
    std::ofstream f(filePath);
    f.write("init\n",5);
    f.flush();
    f.close();
    FileSink sink{filePath.string()};
    ASSERT_TRUE(sink.write("Hello"));
    ASSERT_TRUE(sink.flush());

    std::ifstream file{filePath};
    ASSERT_TRUE(file.is_open());

    std::string line;
    ASSERT_TRUE(std::getline(file, line));
    EXPECT_EQ(line, "init");

    ASSERT_TRUE(std::getline(file, line));
    EXPECT_EQ(line, "Hello");
}

TEST(FileSinkTest, OpenFailure)
{
    TemporaryDirectory dir;
    const auto filePath = dir.path() / "blablahs" / "blah";
    FileSink sink{filePath.string()};
    ASSERT_FALSE(sink.write("Hello"));
}