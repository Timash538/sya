#include <array>
#include <atomic>
#include <concepts>
#include <cstddef>
#include <type_traits>
#include <utility>
#include <bit>
#include <limits>

#include <Cache.hpp>
#include <Slot.hpp>

namespace sya
{

// Bounded MPSC queue.
//
// Multiple producer threads may concurrently push.
// Exactly one consumer thread may pop.
//
// The queue must not be destroyed while any producer
// or the consumer is accessing it.
template <typename T, std::size_t C>
class MPSCQueue
{
    static_assert(C > 0, "MPSCQueue capacity must be greater than 0");

    static_assert(std::has_single_bit(C), "MPSCQueue capacity must be a power of two");

    static constexpr std::size_t kHalfRange = std::size_t{1} << (std::numeric_limits<std::size_t>::digits - 1);

    static_assert(C < kHalfRange, "MPSCQueue capacity must be less than half of size_t range");

  public:
    MPSCQueue()
    {
        for (std::size_t i = 0; i < C; ++i)
        {
            m_queue[i].sequence.store(i, std::memory_order_relaxed);
        }
    }

    ~MPSCQueue()
    {
        auto pos = m_dequeuePos;
        while (true)
        {
            auto& slot = m_queue[pos % C];
            if (slot.sequence.load(std::memory_order_relaxed) == pos + 1)
                slot.storage.destroy();
            else
                return;
            ++pos;
        }
    }

    template <typename... Args>
    [[nodiscard]]
    bool try_emplace(Args&&... args) noexcept
        requires std::constructible_from<T, Args...> && std::is_nothrow_constructible_v<T, Args...>;
    [[nodiscard]]
    bool try_push(T&& item) noexcept
        requires std::constructible_from<T, T&&> && std::is_nothrow_constructible_v<T, T&&>;
    [[nodiscard]]
    bool try_push(const T& item) noexcept
        requires std::constructible_from<T, const T&> && std::is_nothrow_constructible_v<T, const T&>;
    [[nodiscard]]
    bool try_pop(T& item) noexcept
        requires std::is_nothrow_move_assignable_v<T>;

    [[nodiscard]]
    static constexpr std::size_t capacity() noexcept
    {
        return C;
    }

  private:
    struct SequenceSlot
    {
        std::atomic<std::size_t> sequence;
        Slot<T> storage;
    };

    alignas(kCacheLineSize) std::atomic<std::size_t> m_enqueuePos{0};
    alignas(kCacheLineSize) std::size_t m_dequeuePos{0};
    std::array<SequenceSlot, C> m_queue;
};

template <typename T, std::size_t C>
template <typename... Args>
bool MPSCQueue<T, C>::try_emplace(Args&&... args) noexcept
    requires std::constructible_from<T, Args...> && std::is_nothrow_constructible_v<T, Args...>
{
    auto pos = m_enqueuePos.load(std::memory_order_relaxed);
    while (true)
    {
        auto& slot = m_queue[pos % C];

        const auto sequence = slot.sequence.load(std::memory_order_acquire);
        if (sequence == pos)
        {
            if (m_enqueuePos.compare_exchange_weak(pos, pos + 1, std::memory_order_relaxed))
            {
                slot.storage.construct(std::forward<Args>(args)...);
                slot.sequence.store(pos + 1, std::memory_order_release);
                return true;
            }
        }
        else if ((pos - sequence) < kHalfRange)
        {
            return false;
        }
        else
        {
            pos = m_enqueuePos.load(std::memory_order_relaxed);
        }
    }
}

template <typename T, std::size_t C>
bool MPSCQueue<T, C>::try_push(T&& item) noexcept
    requires std::constructible_from<T, T&&> && std::is_nothrow_constructible_v<T, T&&>
{
    return try_emplace(std::move(item));
}

template <typename T, std::size_t C>
bool MPSCQueue<T, C>::try_push(const T& item) noexcept
    requires std::constructible_from<T, const T&> && std::is_nothrow_constructible_v<T, const T&>
{
    return try_emplace(item);
}

template <typename T, std::size_t C>
bool MPSCQueue<T, C>::try_pop(T& item) noexcept
    requires std::is_nothrow_move_assignable_v<T>
{
    const auto pos = m_dequeuePos;
    auto& slot = m_queue[pos % C];

    const auto sequence = slot.sequence.load(std::memory_order_acquire);

    if (sequence != pos + 1)
    {
        return false;
    }
    item = std::move(slot.storage.get());
    slot.storage.destroy();
    slot.sequence.store(pos + C, std::memory_order_release);
    ++m_dequeuePos;
    return true;
}

} // namespace sya