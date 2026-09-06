#include <memory>

#include <Slot.hpp>

#include <gtest/gtest.h>

namespace
{

struct TestObject
{
    explicit TestObject(int value) : value{value} {}

    int value;
};

struct TrackedObject
{
    explicit TrackedObject(bool& destroyed) : destroyed{destroyed} {}

    ~TrackedObject()
    {
        destroyed = true;
    }

    bool& destroyed;
};


} // namespace

TEST(SlotTest, ConstructsAndGetsObject)
{
    using namespace sya;
    Slot<TestObject> slot;
    slot.construct(42);
    auto& object = slot.get();
    EXPECT_EQ(object.value, 42);
    slot.destroy();
}

TEST(SlotTest, DestroyObject)
{
    using namespace sya;
    bool destroyed = false;
    Slot<TrackedObject> slot;
    slot.construct(destroyed);
    EXPECT_EQ(destroyed, false);
    slot.destroy();
    EXPECT_EQ(destroyed, true);
}

TEST(SlotTest, ConstructUniquePtrObject)
{
    using namespace sya;
    Slot<std::unique_ptr<TestObject>> slot;
    slot.construct(std::make_unique<TestObject>(42));
    auto object = std::move(slot.get()); 
    EXPECT_EQ(object->value, 42);
    slot.destroy();
}
