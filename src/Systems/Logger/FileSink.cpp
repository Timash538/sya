#include <FileSink.hpp>

using namespace sya::logging;

bool FileSink::write(std::string_view text)
{
    if (!m_file.is_open())
    {
        m_file.open(m_path, std::ios::app);

        if (!m_file)
            return false;
    }

    m_file.write(text.data(), text.size());
    m_file.put('\n');
    if (m_file.good())
    {
        m_bytesSinceFlush += text.size() + 1;

        if (m_bytesSinceFlush >= FlushThreshold)
            return flush();
        return true;
    }
    return false;
}

bool FileSink::flush()
{
    if (!m_file.is_open())
        return true;

    m_file.flush();

    if (!m_file.good())
        return false;

    m_bytesSinceFlush = 0;
    return true;
}