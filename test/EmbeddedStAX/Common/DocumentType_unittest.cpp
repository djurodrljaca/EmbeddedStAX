#include <gtest/gtest.h>
#include <EmbeddedStAX/Common/DocumentType.h>
#include <EmbeddedStAX/Common/Utf.h>
#include <queue>

using namespace EmbeddedStAX::Common;

//--------------------------------------------------------------------------------------------------
// Test case: EmbeddedStAX::Common::DocumentType
//--------------------------------------------------------------------------------------------------
TEST(EmbeddedStAX_Common_DocumentType, ConstructorTest)
{
    const UnicodeString name = Utf8::toUnicodeString("name");

    const DocumentType documentType(name);

    EXPECT_EQ(name, documentType.name());
}

TEST(EmbeddedStAX_Common_DocumentType, DefaultConstructorTest)
{
    DocumentType documentType;

    EXPECT_EQ(documentType.name(), UnicodeString());
}

TEST(EmbeddedStAX_Common_DocumentType, DataMemberAccessTest)
{
    // Initial values
    DocumentType documentType;
    const UnicodeString name1 = Utf8::toUnicodeString("name1");
    documentType.setName(name1);

    EXPECT_EQ(name1, documentType.name());

    // Changed values
    const UnicodeString name2 = Utf8::toUnicodeString("name2");
    documentType.setName(name2);

    EXPECT_EQ(name2, documentType.name());
}

TEST(EmbeddedStAX_Common_DocumentType, CopyConstructorTest)
{
    const UnicodeString name = Utf8::toUnicodeString("name");

    const DocumentType documentType1(name);
    const DocumentType documentType2(documentType1);

    EXPECT_EQ(documentType1.name(), documentType2.name());
}

TEST(EmbeddedStAX_Common_DocumentType, AssignmentOperatorTest)
{
    const UnicodeString name = Utf8::toUnicodeString("name");

    const DocumentType documentType1(name);
    DocumentType documentType2;
    documentType2 = documentType1;

    EXPECT_EQ(documentType1.name(), documentType2.name());
}

TEST(EmbeddedStAX_Common_DocumentType, ClearingTest)
{
    const UnicodeString name = Utf8::toUnicodeString("name");

    DocumentType documentType(name);
    documentType.clear();

    EXPECT_EQ(documentType.name(), UnicodeString());
}

TEST(EmbeddedStAX_Common_DocumentType, ValidationTest)
{
    DocumentType documentType;

    // Valid name
    documentType.setName(Utf8::toUnicodeString("name"));

    EXPECT_TRUE(documentType.isValid());

    // Invalid name
    documentType.setName(Utf8::toUnicodeString("0name"));

    EXPECT_FALSE(documentType.isValid());
}
