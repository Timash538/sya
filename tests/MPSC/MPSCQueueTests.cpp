#include <gtest/gtest.h>

#include <MPSCQueue.hpp>

TEST(MPSCQueueTest, PopFromEmptyQueueReturnsFalse)
{
    sya::MPSCQueue<int, 4> queue;

    int value = 0;

    EXPECT_FALSE(queue.try_pop(value));
}

TEST(MPSCQueueTest, PushIntoFullQueueReturnsFalse)
{
    sya::MPSCQueue<int, 4> queue;

    int value = 0;
    EXPECT_TRUE(queue.try_push(value));
    EXPECT_TRUE(queue.try_push(value));
    EXPECT_TRUE(queue.try_push(value));
    EXPECT_TRUE(queue.try_push(value));

    EXPECT_FALSE(queue.try_push(value));
}

TEST(MPSCQueueTest, PreservesFIFOOrder)
{
    sya::MPSCQueue<int, 4> queue;

    EXPECT_TRUE(queue.try_push(10));
    EXPECT_TRUE(queue.try_push(20));
    EXPECT_TRUE(queue.try_push(30));

    int value = 0;

    EXPECT_TRUE(queue.try_pop(value));
    EXPECT_EQ(value, 10);

    EXPECT_TRUE(queue.try_pop(value));
    EXPECT_EQ(value, 20);

    EXPECT_TRUE(queue.try_pop(value));
    EXPECT_EQ(value, 30);

    EXPECT_FALSE(queue.try_pop(value));
}

TEST(MPSCQueueTest, WrapAround)
{
    sya::MPSCQueue<int, 4> queue;

    // enqueue pos: 0 -> 1 -> 2 -> 3
    EXPECT_TRUE(queue.try_push(10));
    EXPECT_TRUE(queue.try_push(20));
    EXPECT_TRUE(queue.try_push(30));

    int value = 0;

    // Освобождаем два места.
    // dequeue pos: 0 -> 1 -> 2
    EXPECT_TRUE(queue.try_pop(value));
    EXPECT_EQ(value, 10);

    EXPECT_TRUE(queue.try_pop(value));
    EXPECT_EQ(value, 20);

    // Теперь head дойдёт до конца массива
    // и завернётся обратно в 0.
    EXPECT_TRUE(queue.try_push(40));
    EXPECT_TRUE(queue.try_push(50));

    // В очереди логически должны остаться:
    // 30, 40, 50

    EXPECT_TRUE(queue.try_pop(value));
    EXPECT_EQ(value, 30);

    EXPECT_TRUE(queue.try_pop(value));
    EXPECT_EQ(value, 40);

    EXPECT_TRUE(queue.try_pop(value));
    EXPECT_EQ(value, 50);

    EXPECT_FALSE(queue.try_pop(value));
}

TEST(MPSCQueueTest, ProducerConsumer)
{
    constexpr std::size_t count = 100000;

    struct Message
    {
        int producer{0};
        int num{0};
    };

    sya::MPSCQueue<Message, 1024> queue;

    auto pushAll = [&](int producer)
    {
        for (int i = 0; i < count; ++i)
        {
            while (!queue.try_push({producer, i}))
            {
                // очередь временно заполнена
            }
        }
    };

    std::thread producer1(pushAll, 0);
    std::thread producer2(pushAll, 1);
    std::thread producer3(pushAll, 2);
    std::thread producer4(pushAll, 3);

    std::atomic<bool> failed{false};
    std::array<int, 4> expected{};

    std::thread consumer(
        [&]()
        {
            for (int i = 0; i < count * 4; ++i)
            {
                Message value;

                while (!queue.try_pop(value))
                {
                    // очередь временно пустая
                }

                const auto producer = value.producer;

                if (producer < 0 || producer > 3)
                {
                    failed.store(true, std::memory_order_relaxed);
                }
                else if (value.num != expected[producer])
                {
                    failed.store(true, std::memory_order_relaxed);
                }
                else
                {
                    ++expected[producer];
                }
            }
        });

    producer1.join();
    producer2.join();
    producer3.join();
    producer4.join();
    consumer.join();

    for (const auto value : expected)
    {
        EXPECT_EQ(value, count);
    }

    EXPECT_FALSE(failed.load(std::memory_order_relaxed));
}