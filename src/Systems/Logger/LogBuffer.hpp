#pragma once

#include <algorithm>
#include <charconv>
#include <cstddef>
#include <cstring>
#include <iterator>
#include <limits>
#include <ranges>
#include <span>
#include <string_view>
#include <system_error>
#include <type_traits>

namespace sya::logging
{

struct LogBuffer
{
  public:
    explicit LogBuffer(std::span<char> buffer) noexcept : m_buffer(buffer) {};

    void append(std::string_view text) noexcept
    {
        const auto count = std::min(text.size(), remaining());

        if (count > 0)
        {
            std::memmove(m_buffer.data() + m_size, text.data(), count);

            m_size += count;
        }

        m_truncated |= count < text.size();
    };

    template <typename T>
    void appendValue(const T& value, std::size_t depth = 0) noexcept
    {
        using ValueType = std::remove_cvref_t<decltype(value)>;

        if constexpr (std::is_same_v<bool, ValueType> == true)
        {
            append(value ? "true" : "false");
        }
        else if constexpr (std::is_same_v<ValueType, char>)
        {
            append(std::string_view{&value, 1});
        }
        else if constexpr (std::is_integral_v<ValueType> == true)
        {
            char buffer[std::numeric_limits<ValueType>::digits10 + 3];

            auto result = std::to_chars(std::begin(buffer), std::end(buffer), value);

            if (result.ec == std::errc{})
            {
                append(std::string_view{buffer, static_cast<std::size_t>(result.ptr - buffer)});
            }
            else
            {
                m_truncated = true;
            }
        }
        else if constexpr (std::is_floating_point_v<ValueType>)
        {
            char buffer[128];

            auto result = std::to_chars(std::begin(buffer), std::end(buffer), value);

            if (result.ec == std::errc{})
            {
                append(std::string_view{buffer, static_cast<std::size_t>(result.ptr - buffer)});
            }
            else
            {
                m_truncated = true;
            }
        }
        else if constexpr (std::is_convertible_v<const T&, std::string_view> == true)
        {
            append(std::string_view(value));
        }
        else if constexpr (std::ranges::input_range<const ValueType>)
        {
            static constexpr std::size_t MaxContainerElements = 32;
            static constexpr std::size_t MaxDepth = 3;
            if (depth >= MaxDepth)
            {
                append("[...]");
                m_truncated = true;
                return;
            }
            append("[");

            std::size_t count = 0;

            for (const auto& element : value)
            {
                if (count >= MaxContainerElements)
                {
                    append(", ...");
                    m_truncated = true;
                    break;
                }

                if (remaining() == 0)
                {
                    m_truncated = true;
                    break;
                }

                if (count > 0)
                    append(", ");

                appendValue(element, depth + 1);

                ++count;

                if (m_truncated)
                    break;
            }

            append("]");
        }
        else
            static_assert(std::is_same_v<ValueType, void>, "Unsupported LogBuffer value type");
    }

    std::size_t size() const noexcept
    {
        return m_size;
    }

    std::size_t remaining() const noexcept
    {
        return m_buffer.size() - m_size;
    }

    bool truncated() const noexcept
    {
        return m_truncated;
    }

  private:
    std::span<char> m_buffer;
    std::size_t m_size{0};
    bool m_truncated{false};
};
} // namespace sya::logging