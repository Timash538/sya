#include <SourceInfo.hpp>

using namespace sya::logging;

SourceInfo SourceInfo::parse(std::source_location location) noexcept
{
    std::string_view signature{location.function_name()};

    auto args = signature.find('(');

    // Отдельно обрабатываем operator()
    if (args != signature.npos && signature.substr(args).starts_with("()("))
    {
        args += 2;
    }

    auto name = signature.substr(0, args);

    // Ищем последний :: вне параметров шаблонов
    auto lastScope = [](std::string_view str) noexcept
    {
        std::size_t depth = 0;

        for (std::size_t i = str.size(); i > 0; --i)
        {
            const char ch = str[i - 1];

            if (ch == '>')
                ++depth;
            else if (ch == '<' && depth > 0)
                --depth;

            if (depth == 0 && i >= 2 && str[i - 1] == ':' && str[i - 2] == ':')
            {
                return i - 2;
            }
        }

        return std::string_view::npos;
    };

    // Убираем возвращаемый тип
    std::size_t depth = 0;

    for (std::size_t i = name.size(); i > 0; --i)
    {
        const char ch = name[i - 1];

        if (ch == '>')
            ++depth;
        else if (ch == '<' && depth > 0)
            --depth;
        else if ((ch == ' ' || ch == '\t') && depth == 0)
        {
            name.remove_prefix(i);
            break;
        }
    }

    const auto separator = lastScope(name);

    if (separator == name.npos)
        return {{}, name};

    const auto method = name.substr(separator + 2);

    auto qualifiers = name.substr(0, separator);

    // Оставляем ближайшую область видимости
    const auto previous = lastScope(qualifiers);

    const auto scope = qualifiers.substr(previous == qualifiers.npos ? 0 : previous + 2);

    return {scope, method};
}