#include <CppUTest/TestHarness.h>
#include <CppUTestExt/MockSupport.h>

#include <msgbus/messagebus.h>
#include "mocks/synchronization.hpp"

TEST_GROUP (MsgBusForeachTestGroup) {
    messagebus_t bus;
    messagebus_topic_t foo, bar;

    void setup() override
    {
        int bus_lock;
        messagebus_init(&bus, &bus_lock, nullptr);
        messagebus_topic_init(&foo, nullptr, nullptr, nullptr, 0);
        messagebus_topic_init(&bar, nullptr, nullptr, nullptr, 0);
        messagebus_advertise_topic(&bus, &foo, "foo");
        messagebus_advertise_topic(&bus, &bar, "bar");
    }

    void teardown() override
    {
        lock_mocks_enable(false);
        mock().checkExpectations();
        mock().clear();
    }
};

TEST(MsgBusForeachTestGroup, CanIterate)
{
    mock().expectOneCall("loop").withParameter("topic", &foo);
    mock().expectOneCall("loop").withParameter("topic", &bar);

    MESSAGEBUS_TOPIC_FOREACH (&bus, topic) {
        mock().actualCall("loop").withParameter("topic", topic);
    }
}

TEST(MsgBusForeachTestGroup, CanLock)
{
    lock_mocks_enable(true);
    mock().expectOneCall("messagebus_lock_acquire").withPointerParameter("lock", bus.lock);
    mock().expectOneCall("messagebus_lock_release").withPointerParameter("lock", bus.lock);

    MESSAGEBUS_TOPIC_FOREACH (&bus, topic) {
    }
}

TEST(MsgBusForeachTestGroup, CanBreak)
{
    lock_mocks_enable(true);
    mock().expectOneCall("messagebus_lock_acquire").withPointerParameter("lock", bus.lock);
    mock().expectOneCall("messagebus_lock_release").withPointerParameter("lock", bus.lock);

    MESSAGEBUS_TOPIC_FOREACH (&bus, topic) {
        break;
    }
}
