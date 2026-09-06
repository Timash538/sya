#include <array>
#include <atomic>
#include <concepts>
#include <cstddef>
#include <type_traits>
#include <utility>

#include <Slot.hpp>

namespace sya
{
// Bounded lock-free SPSC queue.
// Exactly one producer and one consumer.
// Queue must not be destroyed while either thread is using it
template <typename T, std::size_t C>
class SPSCQueue
{
    static_assert(C > 0, "SPSCQueue capacity must be greater than 0");

  public:
    ~SPSCQueue()
    {
        auto tail = m_tail.load(std::memory_order_relaxed);
        const auto head = m_head.load(std::memory_order_relaxed);

        while (tail != head)
        {
            m_queue[tail].destroy();
            tail = getNext(tail);
        }
    }

    static constexpr std::size_t capacity() noexcept
    {
        return C;
    }

    template <typename... Args>
    [[nodiscard]]
    bool try_emplace(Args&&... args)
        requires std::constructible_from<T, Args...>;
    [[nodiscard]]
    bool try_push(T&& item)
        requires std::constructible_from<T, T&&>;
    [[nodiscard]]
    bool try_push(const T& item)
        requires std::constructible_from<T, const T&>;
    [[nodiscard]]
    bool try_pop(T& item) noexcept
        requires std::is_nothrow_move_assignable_v<T>;

  private:
    std::size_t getNext(std::size_t idx) const
    {
        if (idx + 1 >= m_queue.size())
            return 0;
        else
            return idx + 1;
    }
    alignas(64) std::atomic<std::size_t> m_head{0};
    alignas(64) std::atomic<std::size_t> m_tail{0};
    std::array<Slot<T>, C + 1> m_queue{};
};

template <typename T, std::size_t C>
bool SPSCQueue<T, C>::try_push(T&& item)
    requires std::constructible_from<T, T&&>
{
    return try_emplace(std::move(item));
}

template <typename T, std::size_t C>
bool SPSCQueue<T, C>::try_push(const T& item)
    requires std::constructible_from<T, const T&>
{
    return try_emplace(item);
}

template <typename T, std::size_t C>
bool SPSCQueue<T, C>::try_pop(T& item) noexcept
    requires std::is_nothrow_move_assignable_v<T>
{
    auto head = m_head.load(std::memory_order_acquire);
    auto tail = m_tail.load(std::memory_order_relaxed);
    auto nextTail = getNext(tail);
    if (tail == head)
        return false;
    item = std::move(m_queue[tail].get());
    m_queue[tail].destroy();
    m_tail.store(nextTail, std::memory_order_release);
    return true;
}
template <typename T, std::size_t C>
template <typename... Args>
bool SPSCQueue<T, C>::try_emplace(Args&&... args)
    requires std::constructible_from<T, Args...>
{
    auto head = m_head.load(std::memory_order_relaxed);
    auto nextHead = getNext(head);

    if (nextHead == m_tail.load(std::memory_order_acquire))
        return false;

    m_queue[head].construct(std::forward<Args>(args)...);

    m_head.store(nextHead, std::memory_order_release);
    return true;
}

} // namespace sya