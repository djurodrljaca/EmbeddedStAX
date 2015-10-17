#include <gtest/gtest.h>
#include <EmbeddedStAX/Common/Common.h>
#include <queue>

using namespace EmbeddedStAX::Common;

//--------------------------------------------------------------------------------------------------
// Test case: EmbeddedStAX::Common::parseDigit()
//--------------------------------------------------------------------------------------------------
struct TestData
{
    TestData(const uint32_t digitCharacter, const uint32_t expectedDigitValue)
        : digitCharacter(digitCharacter), expectedDigitValue(expectedDigitValue)
    {
    }

    uint32_t digitCharacter;
    uint32_t expectedDigitValue;
};

// Testing decimal numbers
TEST(EmbeddedStAX_Common_Common_parseDigit, DecimalPositiveTests)
{
    std::queue<TestData> testDataQueue;

    testDataQueue.push(TestData(static_cast<uint32_t>('0'), 0U));
    testDataQueue.push(TestData(static_cast<uint32_t>('1'), 1U));
    testDataQueue.push(TestData(static_cast<uint32_t>('2'), 2U));
    testDataQueue.push(TestData(static_cast<uint32_t>('3'), 3U));
    testDataQueue.push(TestData(static_cast<uint32_t>('4'), 4U));
    testDataQueue.push(TestData(static_cast<uint32_t>('5'), 5U));
    testDataQueue.push(TestData(static_cast<uint32_t>('6'), 6U));
    testDataQueue.push(TestData(static_cast<uint32_t>('7'), 7U));
    testDataQueue.push(TestData(static_cast<uint32_t>('8'), 8U));
    testDataQueue.push(TestData(static_cast<uint32_t>('9'), 9U));

    while (!testDataQueue.empty())
    {
        const TestData &testData = testDataQueue.front();
        uint32_t digitValue = 0U;
        const bool success = parseDigit(testData.digitCharacter, 10U, &digitValue);

        EXPECT_TRUE(success);
        EXPECT_EQ(testData.expectedDigitValue, digitValue);

        testDataQueue.pop();
    }
}

TEST(EmbeddedStAX_Common_Common_parseDigit, DecimalNegativeTests)
{
    std::queue<uint32_t> testDataQueue;

    testDataQueue.push(static_cast<uint32_t>('0' - 1));
    testDataQueue.push(static_cast<uint32_t>('9' + 1));

    while (!testDataQueue.empty())
    {
        uint32_t digitValue = 0U;
        const bool success = parseDigit(testDataQueue.front(), 10U, &digitValue);

        EXPECT_FALSE(success);

        testDataQueue.pop();
    }
}

// Testing hexadecimal numbers
TEST(EmbeddedStAX_Common_Common_parseDigit, HexadecimalPositiveTests)
{
    std::queue<TestData> testDataQueue;

    testDataQueue.push(TestData(static_cast<uint32_t>('0'), 0U));
    testDataQueue.push(TestData(static_cast<uint32_t>('1'), 1U));
    testDataQueue.push(TestData(static_cast<uint32_t>('2'), 2U));
    testDataQueue.push(TestData(static_cast<uint32_t>('3'), 3U));
    testDataQueue.push(TestData(static_cast<uint32_t>('4'), 4U));
    testDataQueue.push(TestData(static_cast<uint32_t>('5'), 5U));
    testDataQueue.push(TestData(static_cast<uint32_t>('6'), 6U));
    testDataQueue.push(TestData(static_cast<uint32_t>('7'), 7U));
    testDataQueue.push(TestData(static_cast<uint32_t>('8'), 8U));
    testDataQueue.push(TestData(static_cast<uint32_t>('9'), 9U));

    testDataQueue.push(TestData(static_cast<uint32_t>('A'), 0xAU));
    testDataQueue.push(TestData(static_cast<uint32_t>('B'), 0xBU));
    testDataQueue.push(TestData(static_cast<uint32_t>('C'), 0xCU));
    testDataQueue.push(TestData(static_cast<uint32_t>('D'), 0xDU));
    testDataQueue.push(TestData(static_cast<uint32_t>('E'), 0xEU));
    testDataQueue.push(TestData(static_cast<uint32_t>('F'), 0xFU));

    testDataQueue.push(TestData(static_cast<uint32_t>('a'), 0xAU));
    testDataQueue.push(TestData(static_cast<uint32_t>('b'), 0xBU));
    testDataQueue.push(TestData(static_cast<uint32_t>('c'), 0xCU));
    testDataQueue.push(TestData(static_cast<uint32_t>('d'), 0xDU));
    testDataQueue.push(TestData(static_cast<uint32_t>('e'), 0xEU));
    testDataQueue.push(TestData(static_cast<uint32_t>('f'), 0xFU));

    while (!testDataQueue.empty())
    {
        const TestData &testData = testDataQueue.front();
        uint32_t digitValue = 0U;
        const bool success = parseDigit(testData.digitCharacter, 16U, &digitValue);

        EXPECT_TRUE(success);
        EXPECT_EQ(testData.expectedDigitValue, digitValue);

        testDataQueue.pop();
    }
}

TEST(EmbeddedStAX_Common_Common_parseDigit, HexadecimalNegativeTests)
{
    std::queue<uint32_t> testDataQueue;

    testDataQueue.push(static_cast<uint32_t>('0' - 1));
    testDataQueue.push(static_cast<uint32_t>('9' + 1));

    testDataQueue.push(static_cast<uint32_t>('A' - 1));
    testDataQueue.push(static_cast<uint32_t>('F' + 1));

    testDataQueue.push(static_cast<uint32_t>('a' - 1));
    testDataQueue.push(static_cast<uint32_t>('f' + 1));

    while (!testDataQueue.empty())
    {
        uint32_t digitValue = 0U;
        const bool success = parseDigit(testDataQueue.front(), 16U, &digitValue);

        EXPECT_FALSE(success);

        testDataQueue.pop();
    }
}
