#define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING
#include "gtest/gtest.h"
#include "../headers/SharedQueue.h"

using namespace solution;

TEST(SharedQueueCore, HeaderSerializationRoundTrip) {
    SharedQueue q;
    ASSERT_TRUE(q.CreateAsReceiver("test_core.bin", 3, 1));
    EXPECT_TRUE(q.IsValid());
    EXPECT_EQ(q.Capacity(), 3u);
    EXPECT_EQ(q.ReadIndex(), 0u);
    EXPECT_EQ(q.WriteIndex(), 0u);

    string m1 = "a";
    string m2 = "b";
    EXPECT_TRUE(q.PushMessage(m1, false));
    EXPECT_TRUE(q.PushMessage(m2, false));
    EXPECT_EQ(q.WriteIndex(), 2u);

    string out;
    EXPECT_TRUE(q.PopMessage(out, false));
    EXPECT_EQ(out, "a");
    EXPECT_EQ(q.ReadIndex(), 1u);

    EXPECT_TRUE(q.PopMessage(out, false));
    EXPECT_EQ(out, "b");
    EXPECT_EQ(q.ReadIndex(), 2u);

    q.SignalShutdown();
}

TEST(SharedQueueCore, CapacityBlocksOnFullAndEmpty) {
    SharedQueue q;
    ASSERT_TRUE(q.CreateAsReceiver("test_block.bin", 2, 0));
    EXPECT_TRUE(q.PushMessage("x", false));
    EXPECT_TRUE(q.PushMessage("y", false));
    EXPECT_EQ(q.WriteIndex(), 0u);

    string out;
    EXPECT_TRUE(q.PopMessage(out, false));
    EXPECT_EQ(out, "x");
    EXPECT_TRUE(q.PopMessage(out, false));
    EXPECT_EQ(out, "y");
    q.SignalShutdown();
}