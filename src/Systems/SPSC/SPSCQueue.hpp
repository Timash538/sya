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
    bool push(T&& item);
    bool push(const T& item);
    bool pop(T& item);
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
bool SPSCQueue<T, C>::push(T&& item)
{
    auto head = m_head.load(std::memory_order_relaxed);
    auto nextHead = getNext(head);
    if (nextHead == m_tail.load(std::memory_order_acquire))
        return false;
    m_queue[head].construct(std::move(item));
    m_head.store(nextHead, std::memory_order_release);
    return true;
}

template <typename T, std::size_t C>
bool SPSCQueue<T, C>::push(const T& item)
{
    auto head = m_head.load(std::memory_order_relaxed);
    auto nextHead = getNext(head);
    if (nextHead == m_tail.load(std::memory_order_acquire))
        return false;
    m_queue[head].construct(item);
    m_head.store(nextHead, std::memory_order_release);
    return true;
}

template <typename T, std::size_t C>
bool SPSCQueue<T, C>::pop(T& item)
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

} // namespace sya