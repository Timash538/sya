#include <Logger.hpp>

#include <LogFormatter.hpp>

using namespace sya::logging;

Logger& Logger::instance()
{
    static Logger logger;
    return logger;
}

Logger::~Logger()
{
    // Все producer-потоки должны быть остановлены.
    m_worker.request_stop();

    m_cs.release();

    m_worker.join();
}

void Logger::run(std::stop_token stop) noexcept
{
    auto nextFlushTime{std::chrono::steady_clock::now() + std::chrono::seconds(1)};

    while (true)
    {
        constexpr std::size_t kBatchSize{256};
        std::size_t count{0};
        while (count < kBatchSize && m_queue.try_consume([this](LogRecord& record) noexcept { process(record); }))
        {
            ++count;
        }

        if (std::chrono::steady_clock::now() >= nextFlushTime)
        {
            m_fileSink.flush();
            nextFlushTime = std::chrono::steady_clock::now() + std::chrono::seconds(1);
        }

        if (count == kBatchSize)
        {
            continue;
        }

        m_flag.exchange(false, std::memory_order_acq_rel);
        if (m_queue.try_consume([this](LogRecord& record) noexcept { process(record); }))
        {
            continue;
        }

        if (stop.stop_requested())
        {
            m_fileSink.flush();
            break;
        }

        m_cs.try_acquire_until(nextFlushTime);
    }
}

void Logger::process(const LogRecord& record) noexcept
{
    std::array<char, 1024> storage{};

    LogBuffer buffer{storage};

    LogFormatter::format(record, buffer);

    const std::string_view formatted{storage.data(), buffer.size()};

    m_consoleSink.write(formatted);

    if (!m_fileSink.write(formatted))
    {
        // TODO: обработка ошибки записи в файл
    }
}