#include <gtest/gtest.h>

#include <SPSCQueue.hpp>

TEST(SPSCQueueTest, PopFromEmptyQueueReturnsFalse)
{
    sya::SPSCQueue<int, 4> queue;

    int value = 0;

    EXPECT_FALSE(queue.pop(value));
}

TEST(SPSCQueueTest, PushAndPopOneElement)
{
    sya::SPSCQueue<int, 4> queue;

    EXPECT_TRUE(queue.push(42));

    int value = 0;
    EXPECT_TRUE(queue.pop(value));

    EXPECT_EQ(value, 42);
}

TEST(SPSCQueueTest, QueueBecomesFull)
{
    sya::SPSCQueue<int, 4> queue;

    EXPECT_TRUE(queue.push(1));
    EXPECT_TRUE(queue.push(2));
    EXPECT_TRUE(queue.push(3));

    // Capacity физически 4,
    // но один слот мы оставл€ем свободным.
    EXPECT_FALSE(queue.push(4));
}

TEST(SPSCQueueTest, PreservesFIFOOrder)
{
    sya::SPSCQueue<int, 4> queue;

    EXPECT_TRUE(queue.push(10));
    EXPECT_TRUE(queue.push(20));
    EXPECT_TRUE(queue.push(30));

    int value = 0;

    EXPECT_TRUE(queue.pop(value));
    EXPECT_EQ(value, 10);

    EXPECT_TRUE(queue.pop(value));
    EXPECT_EQ(value, 20);

    EXPECT_TRUE(queue.pop(value));
    EXPECT_EQ(value, 30);

    EXPECT_FALSE(queue.pop(value));
}

TEST(SPSCQueueTest, WrapAround)
{
    sya::SPSCQueue<int, 4> queue;

    // head: 0 -> 1 -> 2 -> 3
    EXPECT_TRUE(queue.push(10));
    EXPECT_TRUE(queue.push(20));
    EXPECT_TRUE(queue.push(30));

    int value = 0;

    // ќсвобождаем два места.
    // tail: 0 -> 1 -> 2
    EXPECT_TRUE(queue.pop(value));
    EXPECT_EQ(value, 10);

    EXPECT_TRUE(queue.pop(value));
    EXPECT_EQ(value, 20);

    // “еперь head дойдЄт до конца массива
    // и завернЄтс€ обратно в 0.
    EXPECT_TRUE(queue.push(40));
    EXPECT_TRUE(queue.push(50));

    // ¬ очереди логически должны остатьс€:
    // 30, 40, 50

    EXPECT_TRUE(queue.pop(value));
    EXPECT_EQ(value, 30);

    EXPECT_TRUE(queue.pop(value));
    EXPECT_EQ(value, 40);

    EXPECT_TRUE(queue.pop(value));
    EXPECT_EQ(value, 50);

    EXPECT_FALSE(queue.pop(value));
}

TEST(SPSCQueueTest, ProducerConsumer)
{
    constexpr std::size_t count = 100000;

    sya::SPSCQueue<int, 1024> queue;

    std::atomic<bool> failed{false};

    std::thread producer(
        [&]()
        {
            for (int i = 0; i < count; ++i)
            {
                while (!queue.push(int{i}))
                {
                    // очередь временно заполнена
                }
            }
        });

    std::thread consumer(
        [&]()
        {
            for (int expected = 0; expected < count; ++expected)
            {
                int value;

                while (!queue.pop(value))
                {
                    // очередь временно пуста€
                }

                if (value != expected)
                {
                    failed.store(true, std::memory_order_relaxed);
                    return;
                }
            }
        });

    producer.join();
    consumer.join();

    EXPECT_FALSE(failed.load(std::memory_order_relaxed));
}