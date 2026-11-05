#include <Logger.hpp>

#define LOGI(...)                                                                                                      \
    ::sya::logging::Logger::instance().log(::sya::logging::LogLevel::Info, std::source_location::current(), __VA_ARGS__)

#define LOGT(...)                                                                                                      \
    ::sya::logging::Logger::instance().log(                                                                            \
        ::sya::logging::LogLevel::Trace, std::source_location::current(), __VA_ARGS__)

#define LOGD(...)                                                                                                      \
    ::sya::logging::Logger::instance().log(                                                                            \
        ::sya::logging::LogLevel::Debug, std::source_location::current(), __VA_ARGS__)

#define LOGW(...)                                                                                                      \
    ::sya::logging::Logger::instance().log(                                                                            \
        ::sya::logging::LogLevel::Warning, std::source_location::current(), __VA_ARGS__)

#define LOGE(...)                                                                                                      \
    ::sya::logging::Logger::instance().log(                                                                            \
        ::sya::logging::LogLevel::Error, std::source_location::current(), __VA_ARGS__)

#define LOGC(...)                                                                                                      \
    ::sya::logging::Logger::instance().log(                                                                            \
        ::sya::logging::LogLevel::Critical, std::source_location::current(), __VA_ARGS__)