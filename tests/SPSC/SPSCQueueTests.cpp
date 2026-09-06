#include <gtest/gtest.h>

#include <SPSCQueue.hpp>

TEST(SPSCQueueTest, PopFromEmptyQueueReturnsFalse)
{
    sya::SPSCQueue<int, 4> queue;

    int value = 0;

    EXPECT_FALSE(queue.try_pop(value));
}

TEST(SPSCQueueTest, PushAndPopOneElement)
{
    sya::SPSCQueue<int, 4> queue;

    EXPECT_TRUE(queue.try_push(42));

    int value = 0;
    EXPECT_TRUE(queue.try_pop(value));

    EXPECT_EQ(value, 42);
}

TEST(SPSCQueueTest, PushAndPopOnePtrElement)
{
    sya::SPSCQueue<std::unique_ptr<int>, 4> queue;

    EXPECT_TRUE(queue.try_push(std::make_unique<int>(42)));

    std::unique_ptr<int> value = 0;
    EXPECT_TRUE(queue.try_pop(value));

    ASSERT_NE(value, nullptr);
    EXPECT_EQ(*value, 42);
}

TEST(SPSCQueueTest, QueueBecomesFull)
{
    sya::SPSCQueue<int, 4> queue;

    EXPECT_TRUE(queue.try_push(1));
    EXPECT_TRUE(queue.try_push(2));
    EXPECT_TRUE(queue.try_push(3));
    EXPECT_TRUE(queue.try_push(4));

    // Capacity физически 4,
    // но один слот мы оставл€ем свободным.
    EXPECT_FALSE(queue.try_push(4));
}

TEST(SPSCQueueTest, PreservesFIFOOrder)
{
    sya::SPSCQueue<int, 4> queue;

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

TEST(SPSCQueueTest, DestroysRemainingMoveOnlyElements)
{
    struct TrackedObject
    {
        explicit TrackedObject(int& alive) : alive{alive}
        {
            ++alive;
        }

        ~TrackedObject()
        {
            --alive;
        }

        int& alive;
    };

    int alive = 0;

    {
        sya::SPSCQueue<std::unique_ptr<TrackedObject>, 6> queue;

        queue.try_push(std::make_unique<TrackedObject>(alive));
        queue.try_push(std::make_unique<TrackedObject>(alive));
        queue.try_push(std::make_unique<TrackedObject>(alive));
        queue.try_push(std::make_unique<TrackedObject>(alive));
        queue.try_push(std::make_unique<TrackedObject>(alive));
        EXPECT_EQ(alive, 5);
    }
    EXPECT_EQ(alive, 0);
}

TEST(SPSCQueueTest, WrapAround)
{
    sya::SPSCQueue<int, 4> queue;

    // head: 0 -> 1 -> 2 -> 3
    EXPECT_TRUE(queue.try_push(10));
    EXPECT_TRUE(queue.try_push(20));
    EXPECT_TRUE(queue.try_push(30));

    int value = 0;

    // ќсвобождаем два места.
    // tail: 0 -> 1 -> 2
    EXPECT_TRUE(queue.try_pop(value));
    EXPECT_EQ(value, 10);

    EXPECT_TRUE(queue.try_pop(value));
    EXPECT_EQ(value, 20);

    // “еперь head дойдЄт до конца массива
    // и завернЄтс€ обратно в 0.
    EXPECT_TRUE(queue.try_push(40));
    EXPECT_TRUE(queue.try_push(50));

    // ¬ очереди логически должны остатьс€:
    // 30, 40, 50

    EXPECT_TRUE(queue.try_pop(value));
    EXPECT_EQ(value, 30);

    EXPECT_TRUE(queue.try_pop(value));
    EXPECT_EQ(value, 40);

    EXPECT_TRUE(queue.try_pop(value));
    EXPECT_EQ(value, 50);

    EXPECT_FALSE(queue.try_pop(value));
}

TEST(SPSCQueueTest, DestroysRemainingElementsAfterWrapAround)
{
    struct TrackedObject
    {
        explicit TrackedObject(int& alive) : alive{alive}
        {
            ++alive;
        }

        ~TrackedObject()
        {
            --alive;
        }

        int& alive;
    };

    int alive = 0;

    {
        sya::SPSCQueue<std::unique_ptr<TrackedObject>, 4> queue;

        queue.try_push(std::make_unique<TrackedObject>(alive));
        queue.try_push(std::make_unique<TrackedObject>(alive));
        queue.try_push(std::make_unique<TrackedObject>(alive));

        EXPECT_EQ(alive, 3);

        std::unique_ptr<TrackedObject> tmp;

        queue.try_pop(tmp);
        tmp.reset();

        queue.try_pop(tmp);
        tmp.reset();

        EXPECT_EQ(alive, 1);

        queue.try_push(std::make_unique<TrackedObject>(alive));
        queue.try_push(std::make_unique<TrackedObject>(alive));

        EXPECT_EQ(alive, 3);
    }

    EXPECT_EQ(alive, 0);
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
                while (!queue.try_push(int{i}))
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

                while (!queue.try_pop(value))
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

TEST(SPSCQueueTest, UsesRequestedCapacity)
{
    sya::SPSCQueue<int, 4> queue;

    EXPECT_EQ(queue.capacity(), 4);

    EXPECT_TRUE(queue.try_push(1));
    EXPECT_TRUE(queue.try_push(2));
    EXPECT_TRUE(queue.try_push(3));
    EXPECT_TRUE(queue.try_push(4));

    EXPECT_FALSE(queue.try_push(5));
}