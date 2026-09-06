#include <array>
#include <atomic>
#include <cstddef>
#include <utility>

#include <Slot.hpp>

namespace sya
{
template <typename T, std::size_t C>
class SPSCQueue
{
    static_assert(C > 1, "SPSCQueue capacity must be greater than 1");

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

    template <typename... Args>
    bool try_emplace(Args&&... args);
    bool try_push(T&& item);
    bool try_push(const T& item);
    bool try_pop(T& item);
    
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
    std::array<Slot<T>, C> m_queue{};
};

template <typename T, std::size_t C>
bool SPSCQueue<T, C>::try_push(T&& item)
{
    return try_emplace(std::move(item));
}

template <typename T, std::size_t C>
bool SPSCQueue<T, C>::try_push(const T& item)
{
    return try_emplace(item);
}

template <typename T, std::size_t C>
bool SPSCQueue<T, C>::try_pop(T& item)
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
template <typename T,std::size_t C>
template <typename... Args>
bool SPSCQueue<T, C>::try_emplace(Args&&... args)
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