#pragma once

#include <source_location>
#include <string_view>

namespace sya::logging
{

struct SourceInfo
{
    std::string_view scope;
    std::string_view method;

    static SourceInfo parse(std::source_location location) noexcept;
};
} // namespace sya::logging