#include <gtest/gtest.h>
#include <EmbeddedStAX/Common/Attribute.h>
#include <EmbeddedStAX/Common/Utf.h>
#include <queue>

using namespace EmbeddedStAX::Common;

//--------------------------------------------------------------------------------------------------
// Test case: EmbeddedStAX::Common::Attribute
//--------------------------------------------------------------------------------------------------
TEST(EmbeddedStAX_Common_Attribute, ConstructorTest)
{
    const UnicodeString name = Utf8::toUnicodeString("name");
    const UnicodeString value = Utf8::toUnicodeString("value");
    const QuotationMark qm = QuotationMark_Apostrophe;

    const Attribute attribute(name, value, qm);

    EXPECT_EQ(name, attribute.name());
    EXPECT_EQ(value, attribute.value());
    EXPECT_EQ(qm, attribute.valueQuotationMark());
}

TEST(EmbeddedStAX_Common_Attribute, DefaultConstructorTest)
{
    Attribute attribute;

    EXPECT_EQ(attribute.name(), UnicodeString());
    EXPECT_EQ(attribute.value(), UnicodeString());
    EXPECT_EQ(attribute.valueQuotationMark(), QuotationMark_Quote);
}

TEST(EmbeddedStAX_Common_Attribute, DataMemberAccessTest)
{
    // Initial values
    Attribute attribute;
    const UnicodeString name1 = Utf8::toUnicodeString("name1");
    const UnicodeString value1 = Utf8::toUnicodeString("value1");
    const QuotationMark qm1 = QuotationMark_Apostrophe;
    attribute.setName(name1);
    attribute.setValue(value1, qm1);

    EXPECT_EQ(name1, attribute.name());
    EXPECT_EQ(value1, attribute.value());
    EXPECT_EQ(qm1, attribute.valueQuotationMark());

    // Changed values
    const UnicodeString name2 = Utf8::toUnicodeString("name2");
    const UnicodeString value2 = Utf8::toUnicodeString("value2");
    const QuotationMark qm2 = QuotationMark_Quote;
    attribute.setName(name2);
    attribute.setValue(value2, qm2);

    EXPECT_EQ(name2, attribute.name());
    EXPECT_EQ(value2, attribute.value());
    EXPECT_EQ(qm2, attribute.valueQuotationMark());
}

TEST(EmbeddedStAX_Common_Attribute, CopyConstructorTest)
{
    const UnicodeString name = Utf8::toUnicodeString("name");
    const UnicodeString value = Utf8::toUnicodeString("value");
    const QuotationMark qm = QuotationMark_Apostrophe;

    const Attribute attribute1(name, value, qm);
    const Attribute attribute2(attribute1);

    EXPECT_EQ(attribute1.name(), attribute2.name());
    EXPECT_EQ(attribute1.value(), attribute2.value());
    EXPECT_EQ(attribute1.valueQuotationMark(), attribute2.valueQuotationMark());
}

TEST(EmbeddedStAX_Common_Attribute, AssignmentOperatorTest)
{
    const UnicodeString name = Utf8::toUnicodeString("name");
    const UnicodeString value = Utf8::toUnicodeString("value");
    const QuotationMark qm = QuotationMark_Apostrophe;

    const Attribute attribute1(name, value, qm);
    Attribute attribute2;
    attribute2 = attribute1;

    EXPECT_EQ(attribute1.name(), attribute2.name());
    EXPECT_EQ(attribute1.value(), attribute2.value());
    EXPECT_EQ(attribute1.valueQuotationMark(), attribute2.valueQuotationMark());
}

TEST(EmbeddedStAX_Common_Attribute, ClearingTest)
{
    const UnicodeString name = Utf8::toUnicodeString("name");
    const UnicodeString value = Utf8::toUnicodeString("value");
    const QuotationMark qm = QuotationMark_Apostrophe;

    Attribute attribute(name, value, qm);
    attribute.clear();

    EXPECT_EQ(attribute.name(), UnicodeString());
    EXPECT_EQ(attribute.value(), UnicodeString());
    EXPECT_EQ(attribute.valueQuotationMark(), QuotationMark_None);
}

//--------------------------------------------------------------------------------------------------
// Test case: EmbeddedStAX::Common::AttributeList
//--------------------------------------------------------------------------------------------------
TEST(EmbeddedStAX_Common_AttributeList, AddingTest)
{
    AttributeList attributeList;

    EXPECT_EQ(0U, attributeList.size());

    attributeList.add(Attribute());

    EXPECT_EQ(1U, attributeList.size());
}

TEST(EmbeddedStAX_Common_AttributeList, ClearingTest)
{
    AttributeList attributeList;

    // Add
    attributeList.add(Attribute());

    EXPECT_EQ(1U, attributeList.size());

    // Clear
    attributeList.clear();

    EXPECT_EQ(0U, attributeList.size());
}

TEST(EmbeddedStAX_Common_AttributeList, IterationTest)
{
    AttributeList attributeList;
    std::list<UnicodeString> nameList;
    nameList.push_back(Utf8::toUnicodeString("name1"));
    nameList.push_back(Utf8::toUnicodeString("name2"));
    nameList.push_back(Utf8::toUnicodeString("name3"));

    // Add
    const UnicodeString value = Utf8::toUnicodeString("value");
    const QuotationMark qm = QuotationMark_Apostrophe;

    for (std::list<UnicodeString>::const_iterator it = nameList.begin(); it != nameList.end(); it++)
    {
        attributeList.add(Attribute(*it, value, qm));
    }

    // Iterate
    std::list<UnicodeString>::const_iterator nameIterator = nameList.begin();
    AttributeList::ConstIterator attributeIterator = attributeList.begin();

    for (uint32_t i = 0U; i < nameList.size(); i++)
    {
        ASSERT_NE(nameIterator, nameList.end());
        ASSERT_NE(attributeIterator, attributeList.end());

        EXPECT_EQ(*nameIterator, attributeIterator->name());
        EXPECT_EQ(value, attributeIterator->value());
        EXPECT_EQ(qm, attributeIterator->valueQuotationMark());
        nameIterator++;
        attributeIterator++;
    }
}

TEST(EmbeddedStAX_Common_AttributeList, SearchByNameTest)
{
    AttributeList attributeList;
    std::list<UnicodeString> nameList;
    nameList.push_back(Utf8::toUnicodeString("name1"));
    nameList.push_back(Utf8::toUnicodeString("name2"));
    nameList.push_back(Utf8::toUnicodeString("name3"));

    // Add
    const UnicodeString value = Utf8::toUnicodeString("value");
    const QuotationMark qm = QuotationMark_Apostrophe;

    for (std::list<UnicodeString>::const_iterator it = nameList.begin(); it != nameList.end(); it++)
    {
        attributeList.add(Attribute(*it, value, qm));
    }

    // Search by name
    std::list<UnicodeString>::const_iterator nameIterator = nameList.begin();

    for (uint32_t i = 0U; i < nameList.size(); i++)
    {
        ASSERT_NE(nameIterator, nameList.end());

        const Attribute *attribute = attributeList.attribute(*nameIterator);

        ASSERT_TRUE(attribute != NULL);

        EXPECT_EQ(*nameIterator, attribute->name());
        EXPECT_EQ(value, attribute->value());
        EXPECT_EQ(qm, attribute->valueQuotationMark());
        nameIterator++;
    }

    // Check for failure
    const Attribute *nullAttribute = attributeList.attribute(UnicodeString());
    EXPECT_EQ(NULL, nullAttribute);

    nullAttribute = attributeList.attribute(Utf8::toUnicodeString("nameX"));
    EXPECT_EQ(NULL, nullAttribute);
}

TEST(EmbeddedStAX_Common_AttributeList, CopyConstructorTest)
{
    AttributeList attributeList1;
    std::list<UnicodeString> nameList;
    nameList.push_back(Utf8::toUnicodeString("name1"));
    nameList.push_back(Utf8::toUnicodeString("name2"));
    nameList.push_back(Utf8::toUnicodeString("name3"));

    // Add
    for (std::list<UnicodeString>::const_iterator it = nameList.begin(); it != nameList.end(); it++)
    {
        attributeList1.add(Attribute(*it, Utf8::toUnicodeString("value")));
    }

    // Copy
    AttributeList attributeList2(attributeList1);

    ASSERT_EQ(attributeList1.size(), attributeList2.size());

    // Iterate and check
    AttributeList::ConstIterator attributeIterator1 = attributeList1.begin();
    AttributeList::ConstIterator attributeIterator2 = attributeList2.begin();

    for (uint32_t i = 0U; i < attributeList1.size(); i++)
    {
        ASSERT_NE(attributeIterator1, attributeList1.end());
        ASSERT_NE(attributeIterator2, attributeList2.end());

        EXPECT_EQ(attributeIterator1->name(), attributeIterator2->name());
        EXPECT_EQ(attributeIterator1->value(), attributeIterator2->value());
        EXPECT_EQ(attributeIterator1->valueQuotationMark(),
                  attributeIterator2->valueQuotationMark());
        attributeIterator1++;
        attributeIterator2++;
    }
}

TEST(EmbeddedStAX_Common_AttributeList, AssignmentOperatorTest)
{
    AttributeList attributeList1;
    std::list<UnicodeString> nameList;
    nameList.push_back(Utf8::toUnicodeString("name1"));
    nameList.push_back(Utf8::toUnicodeString("name2"));
    nameList.push_back(Utf8::toUnicodeString("name3"));

    // Add
    for (std::list<UnicodeString>::const_iterator it = nameList.begin(); it != nameList.end(); it++)
    {
        attributeList1.add(Attribute(*it, Utf8::toUnicodeString("value")));
    }

    // Assign
    AttributeList attributeList2;
    attributeList2 = attributeList1;

    ASSERT_EQ(attributeList1.size(), attributeList2.size());

    // Iterate and check
    AttributeList::ConstIterator attributeIterator1 = attributeList1.begin();
    AttributeList::ConstIterator attributeIterator2 = attributeList2.begin();

    for (uint32_t i = 0U; i < attributeList1.size(); i++)
    {
        ASSERT_NE(attributeIterator1, attributeList1.end());
        ASSERT_NE(attributeIterator2, attributeList2.end());

        EXPECT_EQ(attributeIterator1->name(), attributeIterator2->name());
        EXPECT_EQ(attributeIterator1->value(), attributeIterator2->value());
        EXPECT_EQ(attributeIterator1->valueQuotationMark(),
                  attributeIterator2->valueQuotationMark());
        attributeIterator1++;
        attributeIterator2++;
    }
}
