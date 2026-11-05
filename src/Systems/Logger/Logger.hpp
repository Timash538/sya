#pragma once

#include <atomic>
#include <semaphore>
#include <source_location>
#include <string_view>
#include <thread>

#include <ConsoleSink.hpp>
#include <FileSink.hpp>
#include <LogLevel.hpp>
#include <LogRecord.hpp>
#include <MPSCQueue.hpp>

namespace sya::logging
{

class Logger
{
  public:
    static Logger& instance();

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    template <typename... Args>
    void log(LogLevel level, std::source_location location, std::string_view title, Args&&... args) noexcept
    {
        if (m_queue.try_emplace(level, location, title, std::forward<Args>(args)...))
        {
            if (!m_flag.exchange(true, std::memory_order_acq_rel))
            {
                m_cs.release();
            }
        }
    }

  private:
    Logger() : m_worker([this](std::stop_token stop) noexcept { run(stop); }) {}

    ~Logger();

    void run(std::stop_token stop) noexcept;

    void process(const LogRecord& record) noexcept;

    sya::MPSCQueue<LogRecord, 8192> m_queue;

    ConsoleSink m_consoleSink;
    FileSink m_fileSink{"sya.log"};

    std::counting_semaphore<> m_cs{0};
    std::atomic<bool> m_flag{false};

    std::jthread m_worker;
};
} // namespace sya::logging