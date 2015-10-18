#include <gtest/gtest.h>
#include <EmbeddedStAX/Common/ProcessingInstruction.h>
#include <EmbeddedStAX/Common/Utf.h>
#include <queue>

using namespace EmbeddedStAX::Common;

//--------------------------------------------------------------------------------------------------
// Test case: EmbeddedStAX::Common::ProcessingInstruction
//--------------------------------------------------------------------------------------------------
TEST(EmbeddedStAX_Common_ProcessingInstruction, ConstructorTest)
{
    const UnicodeString target = Utf8::toUnicodeString("target");
    const UnicodeString data = Utf8::toUnicodeString("data");

    const ProcessingInstruction pi(target, data);

    EXPECT_EQ(target, pi.piTarget());
    EXPECT_EQ(data, pi.piData());
}

TEST(EmbeddedStAX_Common_ProcessingInstruction, DefaultConstructorTest)
{
    ProcessingInstruction pi;

    EXPECT_EQ(pi.piTarget(), UnicodeString());
    EXPECT_EQ(pi.piData(), UnicodeString());
}

TEST(EmbeddedStAX_Common_ProcessingInstruction, DataMemberAccessTest)
{
    // Initial values
    ProcessingInstruction pi;
    const UnicodeString target1 = Utf8::toUnicodeString("target1");
    const UnicodeString data1 = Utf8::toUnicodeString("data1");
    pi.setPiTarget(target1);
    pi.setPiData(data1);

    EXPECT_EQ(target1, pi.piTarget());
    EXPECT_EQ(data1, pi.piData());

    // Changed values
    const UnicodeString target2 = Utf8::toUnicodeString("target2");
    const UnicodeString data2 = Utf8::toUnicodeString("data2");
    pi.setPiTarget(target2);
    pi.setPiData(data2);

    EXPECT_EQ(target2, pi.piTarget());
    EXPECT_EQ(data2, pi.piData());
}

TEST(EmbeddedStAX_Common_ProcessingInstruction, CopyConstructorTest)
{
    const UnicodeString target = Utf8::toUnicodeString("target");
    const UnicodeString data = Utf8::toUnicodeString("data");

    const ProcessingInstruction pi1(target, data);
    const ProcessingInstruction pi2(pi1);

    EXPECT_EQ(pi1.piTarget(), pi2.piTarget());
    EXPECT_EQ(pi1.piData(), pi2.piData());
}

TEST(EmbeddedStAX_Common_ProcessingInstruction, AssignmentOperatorTest)
{
    const UnicodeString target = Utf8::toUnicodeString("target");
    const UnicodeString data = Utf8::toUnicodeString("data");

    const ProcessingInstruction pi1(target, data);
    ProcessingInstruction pi2;
    pi2 = pi1;

    EXPECT_EQ(pi1.piTarget(), pi2.piTarget());
    EXPECT_EQ(pi1.piData(), pi2.piData());
}

TEST(EmbeddedStAX_Common_ProcessingInstruction, ClearingTest)
{
    const UnicodeString target = Utf8::toUnicodeString("target");
    const UnicodeString data = Utf8::toUnicodeString("data");

    ProcessingInstruction pi(target, data);
    pi.clear();

    EXPECT_EQ(pi.piTarget(), UnicodeString());
    EXPECT_EQ(pi.piData(), UnicodeString());
}

TEST(EmbeddedStAX_Common_ProcessingInstruction, ValidationTest)
{
    ProcessingInstruction pi;

    // Valid target and data
    pi.setPiTarget(Utf8::toUnicodeString("target"));
    pi.setPiData(Utf8::toUnicodeString("data"));

    EXPECT_TRUE(pi.isValid());

    // Empty data
    pi.setPiData(UnicodeString());

    EXPECT_TRUE(pi.isValid());

    // Empty target
    pi.setPiTarget(UnicodeString());

    EXPECT_FALSE(pi.isValid());

    // Invalid target
    pi.setPiTarget(Utf8::toUnicodeString("0target"));

    EXPECT_FALSE(pi.isValid());

    // Invalid target
    pi.setPiTarget(Utf8::toUnicodeString("xml"));

    EXPECT_FALSE(pi.isValid());

    // Invalid target
    pi.setPiTarget(Utf8::toUnicodeString("XML"));

    EXPECT_FALSE(pi.isValid());

    // Invalid data
    pi.setPiData(Utf8::toUnicodeString("?>"));

    EXPECT_FALSE(pi.isValid());
}
