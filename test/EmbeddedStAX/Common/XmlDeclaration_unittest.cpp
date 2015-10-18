#include <gtest/gtest.h>
#include <EmbeddedStAX/Common/XmlDeclaration.h>
#include <EmbeddedStAX/Common/Utf.h>
#include <queue>

using namespace EmbeddedStAX::Common;

//--------------------------------------------------------------------------------------------------
// Test case: EmbeddedStAX::Common::XmlDeclaration
//--------------------------------------------------------------------------------------------------
TEST(EmbeddedStAX_Common_XmlDeclaration, ConstructorTest)
{
    const XmlDeclaration::Version version = XmlDeclaration::Version_v1_0;
    const XmlDeclaration::Encoding encoding = XmlDeclaration::Encoding_Utf8;
    const XmlDeclaration::Standalone standalone = XmlDeclaration::Standalone_Yes;

    const XmlDeclaration xmlDeclaration(version, encoding, standalone);

    EXPECT_EQ(version, xmlDeclaration.version());
    EXPECT_EQ(encoding, xmlDeclaration.encoding());
    EXPECT_EQ(standalone, xmlDeclaration.standalone());
}

TEST(EmbeddedStAX_Common_XmlDeclaration, DefaultConstructorTest)
{
    const XmlDeclaration xmlDeclaration;

    EXPECT_EQ(XmlDeclaration::Version_None, xmlDeclaration.version());
    EXPECT_EQ(XmlDeclaration::Encoding_None, xmlDeclaration.encoding());
    EXPECT_EQ(XmlDeclaration::Standalone_None, xmlDeclaration.standalone());
}

TEST(EmbeddedStAX_Common_XmlDeclaration, DataMemberAccessTest)
{
    // Initial values
    XmlDeclaration xmlDeclaration;
    const XmlDeclaration::Version version1 = XmlDeclaration::Version_v1_0;
    const XmlDeclaration::Encoding encoding1 = XmlDeclaration::Encoding_Utf8;
    const XmlDeclaration::Standalone standalone1 = XmlDeclaration::Standalone_Yes;
    xmlDeclaration.setVersion(version1);
    xmlDeclaration.setEncoding(encoding1);
    xmlDeclaration.setStandalone(standalone1);

    EXPECT_EQ(version1, xmlDeclaration.version());
    EXPECT_EQ(encoding1, xmlDeclaration.encoding());
    EXPECT_EQ(standalone1, xmlDeclaration.standalone());

    // Changed values
    const XmlDeclaration::Version version2 = XmlDeclaration::Version_None;
    const XmlDeclaration::Encoding encoding2 = XmlDeclaration::Encoding_None;
    const XmlDeclaration::Standalone standalone2 = XmlDeclaration::Standalone_None;
    xmlDeclaration.setVersion(version2);
    xmlDeclaration.setEncoding(encoding2);
    xmlDeclaration.setStandalone(standalone2);

    EXPECT_EQ(version2, xmlDeclaration.version());
    EXPECT_EQ(encoding2, xmlDeclaration.encoding());
    EXPECT_EQ(standalone2, xmlDeclaration.standalone());
}

TEST(EmbeddedStAX_Common_XmlDeclaration, CopyConstructorTest)
{
    const XmlDeclaration::Version version = XmlDeclaration::Version_v1_0;
    const XmlDeclaration::Encoding encoding = XmlDeclaration::Encoding_Utf8;
    const XmlDeclaration::Standalone standalone = XmlDeclaration::Standalone_Yes;

    const XmlDeclaration xmlDeclaration1(version, encoding, standalone);
    const XmlDeclaration xmlDeclaration2(xmlDeclaration1);

    EXPECT_EQ(version, xmlDeclaration2.version());
    EXPECT_EQ(encoding, xmlDeclaration2.encoding());
    EXPECT_EQ(standalone, xmlDeclaration2.standalone());
}

TEST(EmbeddedStAX_Common_XmlDeclaration, AssignmentOperatorTest)
{
    const XmlDeclaration::Version version = XmlDeclaration::Version_v1_0;
    const XmlDeclaration::Encoding encoding = XmlDeclaration::Encoding_Utf8;
    const XmlDeclaration::Standalone standalone = XmlDeclaration::Standalone_Yes;

    // Assignment
    const XmlDeclaration xmlDeclaration1(version, encoding, standalone);
    XmlDeclaration xmlDeclaration2;
    xmlDeclaration2 = xmlDeclaration1;

    EXPECT_EQ(version, xmlDeclaration2.version());
    EXPECT_EQ(encoding, xmlDeclaration2.encoding());
    EXPECT_EQ(standalone, xmlDeclaration2.standalone());

    // Self-assignment
    xmlDeclaration2 = xmlDeclaration2;

    EXPECT_EQ(version, xmlDeclaration2.version());
    EXPECT_EQ(encoding, xmlDeclaration2.encoding());
    EXPECT_EQ(standalone, xmlDeclaration2.standalone());
}

TEST(EmbeddedStAX_Common_XmlDeclaration, ClearingTest)
{
    const XmlDeclaration::Version version = XmlDeclaration::Version_v1_0;
    const XmlDeclaration::Encoding encoding = XmlDeclaration::Encoding_Utf8;
    const XmlDeclaration::Standalone standalone = XmlDeclaration::Standalone_Yes;

    XmlDeclaration xmlDeclaration(version, encoding, standalone);
    xmlDeclaration.clear();

    EXPECT_EQ(XmlDeclaration::Version_None, xmlDeclaration.version());
    EXPECT_EQ(XmlDeclaration::Encoding_None, xmlDeclaration.encoding());
    EXPECT_EQ(XmlDeclaration::Standalone_None, xmlDeclaration.standalone());
}

TEST(EmbeddedStAX_Common_XmlDeclaration, ValidationTest)
{
    XmlDeclaration xmlDeclaration;

    // All valid
    xmlDeclaration.setVersion(XmlDeclaration::Version_v1_0);
    xmlDeclaration.setEncoding(XmlDeclaration::Encoding_Utf8);
    xmlDeclaration.setStandalone(XmlDeclaration::XmlDeclaration::Standalone_Yes);

    EXPECT_TRUE(xmlDeclaration.isValid());

    // Encoding: none
    xmlDeclaration.setVersion(XmlDeclaration::Version_v1_0);
    xmlDeclaration.setEncoding(XmlDeclaration::Encoding_None);
    xmlDeclaration.setStandalone(XmlDeclaration::XmlDeclaration::Standalone_Yes);

    EXPECT_TRUE(xmlDeclaration.isValid());

    // Standalone: none
    xmlDeclaration.setVersion(XmlDeclaration::Version_v1_0);
    xmlDeclaration.setEncoding(XmlDeclaration::Encoding_Utf8);
    xmlDeclaration.setStandalone(XmlDeclaration::Standalone_None);

    EXPECT_TRUE(xmlDeclaration.isValid());

    // Version: none
    xmlDeclaration.setVersion(XmlDeclaration::Version_None);
    xmlDeclaration.setEncoding(XmlDeclaration::Encoding_Utf8);
    xmlDeclaration.setStandalone(XmlDeclaration::XmlDeclaration::Standalone_Yes);

    EXPECT_FALSE(xmlDeclaration.isValid());

    // Version: invalid
    xmlDeclaration.setVersion(XmlDeclaration::Version_Invalid);
    xmlDeclaration.setEncoding(XmlDeclaration::Encoding_Utf8);
    xmlDeclaration.setStandalone(XmlDeclaration::XmlDeclaration::Standalone_Yes);

    EXPECT_FALSE(xmlDeclaration.isValid());

    // Encoding: invalid
    xmlDeclaration.setVersion(XmlDeclaration::Version_v1_0);
    xmlDeclaration.setEncoding(XmlDeclaration::Encoding_Invalid);
    xmlDeclaration.setStandalone(XmlDeclaration::XmlDeclaration::Standalone_Yes);

    EXPECT_FALSE(xmlDeclaration.isValid());

    // Standalone: invalid
    xmlDeclaration.setVersion(XmlDeclaration::Version_v1_0);
    xmlDeclaration.setEncoding(XmlDeclaration::Encoding_Utf8);
    xmlDeclaration.setStandalone(XmlDeclaration::Standalone_Invalid);

    EXPECT_FALSE(xmlDeclaration.isValid());

    // All invalid
    xmlDeclaration.setVersion(XmlDeclaration::Version_Invalid);
    xmlDeclaration.setEncoding(XmlDeclaration::Encoding_Invalid);
    xmlDeclaration.setStandalone(XmlDeclaration::XmlDeclaration::Standalone_Invalid);

    EXPECT_FALSE(xmlDeclaration.isValid());
}

TEST(EmbeddedStAX_Common_XmlDeclaration, FromPiDataWithApostropheTest)
{
    XmlDeclaration xmlDeclaration;

    // One parameter *******************************************************************************
    // Valid version ("v1.0")
    xmlDeclaration = XmlDeclaration::fromPiData(Utf8::toUnicodeString("version='1.0'"));

    EXPECT_TRUE(xmlDeclaration.isValid());
    EXPECT_EQ(XmlDeclaration::Version_v1_0, xmlDeclaration.version());
    EXPECT_EQ(XmlDeclaration::Encoding_None, xmlDeclaration.encoding());
    EXPECT_EQ(XmlDeclaration::Standalone_None, xmlDeclaration.standalone());

    // Invalid version
    xmlDeclaration = XmlDeclaration::fromPiData(Utf8::toUnicodeString("version='1.1'"));

    EXPECT_FALSE(xmlDeclaration.isValid());
    EXPECT_EQ(XmlDeclaration::Version_Invalid, xmlDeclaration.version());
    EXPECT_EQ(XmlDeclaration::Encoding_None, xmlDeclaration.encoding());
    EXPECT_EQ(XmlDeclaration::Standalone_None, xmlDeclaration.standalone());

    // Valid encoding ("UTF-8")
    xmlDeclaration = XmlDeclaration::fromPiData(Utf8::toUnicodeString("encoding='UTF-8'"));

    EXPECT_FALSE(xmlDeclaration.isValid());
    EXPECT_EQ(XmlDeclaration::Version_None, xmlDeclaration.version());
    EXPECT_EQ(XmlDeclaration::Encoding_Utf8, xmlDeclaration.encoding());
    EXPECT_EQ(XmlDeclaration::Standalone_None, xmlDeclaration.standalone());

    // Valid standalone ("yes")
    xmlDeclaration = XmlDeclaration::fromPiData(Utf8::toUnicodeString("standalone='yes'"));

    EXPECT_FALSE(xmlDeclaration.isValid());
    EXPECT_EQ(XmlDeclaration::Version_None, xmlDeclaration.version());
    EXPECT_EQ(XmlDeclaration::Encoding_None, xmlDeclaration.encoding());
    EXPECT_EQ(XmlDeclaration::Standalone_Yes, xmlDeclaration.standalone());

    // Two parameters ******************************************************************************
    // Valid version ("v1.0") and encoding ("UTF-8")
    xmlDeclaration = XmlDeclaration::fromPiData(
                         Utf8::toUnicodeString("version='1.0' encoding='UTF-8'"));

    EXPECT_TRUE(xmlDeclaration.isValid());
    EXPECT_EQ(XmlDeclaration::Version_v1_0, xmlDeclaration.version());
    EXPECT_EQ(XmlDeclaration::Encoding_Utf8, xmlDeclaration.encoding());
    EXPECT_EQ(XmlDeclaration::Standalone_None, xmlDeclaration.standalone());

    // Valid version ("v1.0") and encoding ("utf-8")
    xmlDeclaration = XmlDeclaration::fromPiData(
                         Utf8::toUnicodeString("version='1.0' encoding='utf-8'"));

    EXPECT_TRUE(xmlDeclaration.isValid());
    EXPECT_EQ(XmlDeclaration::Version_v1_0, xmlDeclaration.version());
    EXPECT_EQ(XmlDeclaration::Encoding_Utf8, xmlDeclaration.encoding());
    EXPECT_EQ(XmlDeclaration::Standalone_None, xmlDeclaration.standalone());

    // Valid version ("v1.0") and invalid encoding
    xmlDeclaration = XmlDeclaration::fromPiData(
                         Utf8::toUnicodeString("version='1.0' encoding='utf-'"));

    EXPECT_FALSE(xmlDeclaration.isValid());
    EXPECT_EQ(XmlDeclaration::Version_v1_0, xmlDeclaration.version());
    EXPECT_EQ(XmlDeclaration::Encoding_Invalid, xmlDeclaration.encoding());
    EXPECT_EQ(XmlDeclaration::Standalone_None, xmlDeclaration.standalone());

    // Invalid version and valid encoding ("UTF-8")
    xmlDeclaration = XmlDeclaration::fromPiData(
                         Utf8::toUnicodeString("version='1.1' encoding='UTF-8'"));

    EXPECT_FALSE(xmlDeclaration.isValid());
    EXPECT_EQ(XmlDeclaration::Version_Invalid, xmlDeclaration.version());
    EXPECT_EQ(XmlDeclaration::Encoding_Utf8, xmlDeclaration.encoding());
    EXPECT_EQ(XmlDeclaration::Standalone_None, xmlDeclaration.standalone());

    // Invalid version and encoding
    xmlDeclaration = XmlDeclaration::fromPiData(
                         Utf8::toUnicodeString("version='1.1' encoding='UTF-'"));

    EXPECT_FALSE(xmlDeclaration.isValid());
    EXPECT_EQ(XmlDeclaration::Version_Invalid, xmlDeclaration.version());
    EXPECT_EQ(XmlDeclaration::Encoding_Invalid, xmlDeclaration.encoding());
    EXPECT_EQ(XmlDeclaration::Standalone_None, xmlDeclaration.standalone());

    // Valid version ("v1.0") and standalone ("yes")
    xmlDeclaration = XmlDeclaration::fromPiData(
                         Utf8::toUnicodeString("version='1.0' standalone='yes'"));

    EXPECT_TRUE(xmlDeclaration.isValid());
    EXPECT_EQ(XmlDeclaration::Version_v1_0, xmlDeclaration.version());
    EXPECT_EQ(XmlDeclaration::Encoding_None, xmlDeclaration.encoding());
    EXPECT_EQ(XmlDeclaration::Standalone_Yes, xmlDeclaration.standalone());

    // Valid version ("v1.0") and standalone ("no")
    xmlDeclaration = XmlDeclaration::fromPiData(
                         Utf8::toUnicodeString("version='1.0' standalone='no'"));

    EXPECT_TRUE(xmlDeclaration.isValid());
    EXPECT_EQ(XmlDeclaration::Version_v1_0, xmlDeclaration.version());
    EXPECT_EQ(XmlDeclaration::Encoding_None, xmlDeclaration.encoding());
    EXPECT_EQ(XmlDeclaration::Standalone_No, xmlDeclaration.standalone());

    // Valid version ("v1.0") and invalid standalone
    xmlDeclaration = XmlDeclaration::fromPiData(
                         Utf8::toUnicodeString("version='1.0' standalone='Yes'"));

    EXPECT_FALSE(xmlDeclaration.isValid());
    EXPECT_EQ(XmlDeclaration::Version_v1_0, xmlDeclaration.version());
    EXPECT_EQ(XmlDeclaration::Encoding_None, xmlDeclaration.encoding());
    EXPECT_EQ(XmlDeclaration::Standalone_Invalid, xmlDeclaration.standalone());

    // Valid version ("v1.0") and invalid standalone
    xmlDeclaration = XmlDeclaration::fromPiData(
                         Utf8::toUnicodeString("version='1.0' standalone='No'"));

    EXPECT_FALSE(xmlDeclaration.isValid());
    EXPECT_EQ(XmlDeclaration::Version_v1_0, xmlDeclaration.version());
    EXPECT_EQ(XmlDeclaration::Encoding_None, xmlDeclaration.encoding());
    EXPECT_EQ(XmlDeclaration::Standalone_Invalid, xmlDeclaration.standalone());

    // Invalid version and valid standalone ("no")
    xmlDeclaration = XmlDeclaration::fromPiData(
                         Utf8::toUnicodeString("version='1.1' standalone='no'"));

    EXPECT_FALSE(xmlDeclaration.isValid());
    EXPECT_EQ(XmlDeclaration::Version_Invalid, xmlDeclaration.version());
    EXPECT_EQ(XmlDeclaration::Encoding_None, xmlDeclaration.encoding());
    EXPECT_EQ(XmlDeclaration::Standalone_No, xmlDeclaration.standalone());

    // Invalid version and standalone
    xmlDeclaration = XmlDeclaration::fromPiData(
                         Utf8::toUnicodeString("version='1.1' standalone='Yes'"));

    EXPECT_FALSE(xmlDeclaration.isValid());
    EXPECT_EQ(XmlDeclaration::Version_Invalid, xmlDeclaration.version());
    EXPECT_EQ(XmlDeclaration::Encoding_None, xmlDeclaration.encoding());
    EXPECT_EQ(XmlDeclaration::Standalone_Invalid, xmlDeclaration.standalone());

    // Three parameters ****************************************************************************
    // Valid version ("v1.0"), valid encoding ("UTF-8") and valid standalone ("yes")
    xmlDeclaration = XmlDeclaration::fromPiData(
                         Utf8::toUnicodeString("version='1.0' encoding='UTF-8' standalone='yes'"));

    EXPECT_TRUE(xmlDeclaration.isValid());
    EXPECT_EQ(XmlDeclaration::Version_v1_0, xmlDeclaration.version());
    EXPECT_EQ(XmlDeclaration::Encoding_Utf8, xmlDeclaration.encoding());
    EXPECT_EQ(XmlDeclaration::Standalone_Yes, xmlDeclaration.standalone());

    // Invalid version, valid encoding ("UTF-8") and valid standalone ("yes")
    xmlDeclaration = XmlDeclaration::fromPiData(
                         Utf8::toUnicodeString("version='1.1' encoding='UTF-8' standalone='yes'"));

    EXPECT_FALSE(xmlDeclaration.isValid());
    EXPECT_EQ(XmlDeclaration::Version_Invalid, xmlDeclaration.version());
    EXPECT_EQ(XmlDeclaration::Encoding_Utf8, xmlDeclaration.encoding());
    EXPECT_EQ(XmlDeclaration::Standalone_Yes, xmlDeclaration.standalone());

    // Valid version ("v1.0"), invalid encoding and valid standalone ("yes)
    xmlDeclaration = XmlDeclaration::fromPiData(
                         Utf8::toUnicodeString("version='1.0' encoding='UTF-' standalone='yes'"));

    EXPECT_FALSE(xmlDeclaration.isValid());
    EXPECT_EQ(XmlDeclaration::Version_v1_0, xmlDeclaration.version());
    EXPECT_EQ(XmlDeclaration::Encoding_Invalid, xmlDeclaration.encoding());
    EXPECT_EQ(XmlDeclaration::Standalone_Yes, xmlDeclaration.standalone());

    // Valid version ("v1.0"), valid encoding ("UTF-8") and invalid standalone
    xmlDeclaration = XmlDeclaration::fromPiData(
                         Utf8::toUnicodeString("version='1.0' encoding='UTF-8' standalone='Yes'"));

    EXPECT_FALSE(xmlDeclaration.isValid());
    EXPECT_EQ(XmlDeclaration::Version_v1_0, xmlDeclaration.version());
    EXPECT_EQ(XmlDeclaration::Encoding_Utf8, xmlDeclaration.encoding());
    EXPECT_EQ(XmlDeclaration::Standalone_Invalid, xmlDeclaration.standalone());
}

TEST(EmbeddedStAX_Common_XmlDeclaration, FromPiDataWithQuoteTest)
{
    XmlDeclaration xmlDeclaration;

    // One parameter *******************************************************************************
    // Valid version ("v1.0")
    xmlDeclaration = XmlDeclaration::fromPiData(Utf8::toUnicodeString("version=\"1.0\""));

    EXPECT_TRUE(xmlDeclaration.isValid());
    EXPECT_EQ(XmlDeclaration::Version_v1_0, xmlDeclaration.version());
    EXPECT_EQ(XmlDeclaration::Encoding_None, xmlDeclaration.encoding());
    EXPECT_EQ(XmlDeclaration::Standalone_None, xmlDeclaration.standalone());

    // Invalid version
    xmlDeclaration = XmlDeclaration::fromPiData(Utf8::toUnicodeString("version=\"1.1\""));

    EXPECT_FALSE(xmlDeclaration.isValid());
    EXPECT_EQ(XmlDeclaration::Version_Invalid, xmlDeclaration.version());
    EXPECT_EQ(XmlDeclaration::Encoding_None, xmlDeclaration.encoding());
    EXPECT_EQ(XmlDeclaration::Standalone_None, xmlDeclaration.standalone());

    // Valid encoding ("UTF-8")
    xmlDeclaration = XmlDeclaration::fromPiData(Utf8::toUnicodeString("encoding=\"UTF-8\""));

    EXPECT_FALSE(xmlDeclaration.isValid());
    EXPECT_EQ(XmlDeclaration::Version_None, xmlDeclaration.version());
    EXPECT_EQ(XmlDeclaration::Encoding_Utf8, xmlDeclaration.encoding());
    EXPECT_EQ(XmlDeclaration::Standalone_None, xmlDeclaration.standalone());

    // Valid standalone ("yes")
    xmlDeclaration = XmlDeclaration::fromPiData(Utf8::toUnicodeString("standalone=\"yes\""));

    EXPECT_FALSE(xmlDeclaration.isValid());
    EXPECT_EQ(XmlDeclaration::Version_None, xmlDeclaration.version());
    EXPECT_EQ(XmlDeclaration::Encoding_None, xmlDeclaration.encoding());
    EXPECT_EQ(XmlDeclaration::Standalone_Yes, xmlDeclaration.standalone());

    // Two parameters ******************************************************************************
    // Valid version ("v1.0") and encoding ("UTF-8")
    xmlDeclaration = XmlDeclaration::fromPiData(
                         Utf8::toUnicodeString("version=\"1.0\" encoding=\"UTF-8\""));

    EXPECT_TRUE(xmlDeclaration.isValid());
    EXPECT_EQ(XmlDeclaration::Version_v1_0, xmlDeclaration.version());
    EXPECT_EQ(XmlDeclaration::Encoding_Utf8, xmlDeclaration.encoding());
    EXPECT_EQ(XmlDeclaration::Standalone_None, xmlDeclaration.standalone());

    // Valid version ("v1.0") and encoding ("utf-8")
    xmlDeclaration = XmlDeclaration::fromPiData(
                         Utf8::toUnicodeString("version=\"1.0\" encoding=\"utf-8\""));

    EXPECT_TRUE(xmlDeclaration.isValid());
    EXPECT_EQ(XmlDeclaration::Version_v1_0, xmlDeclaration.version());
    EXPECT_EQ(XmlDeclaration::Encoding_Utf8, xmlDeclaration.encoding());
    EXPECT_EQ(XmlDeclaration::Standalone_None, xmlDeclaration.standalone());

    // Valid version ("v1.0") and invalid encoding
    xmlDeclaration = XmlDeclaration::fromPiData(
                         Utf8::toUnicodeString("version=\"1.0\" encoding=\"utf-\""));

    EXPECT_FALSE(xmlDeclaration.isValid());
    EXPECT_EQ(XmlDeclaration::Version_v1_0, xmlDeclaration.version());
    EXPECT_EQ(XmlDeclaration::Encoding_Invalid, xmlDeclaration.encoding());
    EXPECT_EQ(XmlDeclaration::Standalone_None, xmlDeclaration.standalone());

    // Invalid version and valid encoding ("UTF-8")
    xmlDeclaration = XmlDeclaration::fromPiData(
                         Utf8::toUnicodeString("version=\"1.1\" encoding=\"UTF-8\""));

    EXPECT_FALSE(xmlDeclaration.isValid());
    EXPECT_EQ(XmlDeclaration::Version_Invalid, xmlDeclaration.version());
    EXPECT_EQ(XmlDeclaration::Encoding_Utf8, xmlDeclaration.encoding());
    EXPECT_EQ(XmlDeclaration::Standalone_None, xmlDeclaration.standalone());

    // Invalid version and encoding
    xmlDeclaration = XmlDeclaration::fromPiData(
                         Utf8::toUnicodeString("version=\"1.1\" encoding=\"UTF-\""));

    EXPECT_FALSE(xmlDeclaration.isValid());
    EXPECT_EQ(XmlDeclaration::Version_Invalid, xmlDeclaration.version());
    EXPECT_EQ(XmlDeclaration::Encoding_Invalid, xmlDeclaration.encoding());
    EXPECT_EQ(XmlDeclaration::Standalone_None, xmlDeclaration.standalone());

    // Valid version ("v1.0") and standalone ("yes")
    xmlDeclaration = XmlDeclaration::fromPiData(
                         Utf8::toUnicodeString("version=\"1.0\" standalone=\"yes\""));

    EXPECT_TRUE(xmlDeclaration.isValid());
    EXPECT_EQ(XmlDeclaration::Version_v1_0, xmlDeclaration.version());
    EXPECT_EQ(XmlDeclaration::Encoding_None, xmlDeclaration.encoding());
    EXPECT_EQ(XmlDeclaration::Standalone_Yes, xmlDeclaration.standalone());

    // Valid version ("v1.0") and standalone ("no")
    xmlDeclaration = XmlDeclaration::fromPiData(
                         Utf8::toUnicodeString("version=\"1.0\" standalone=\"no\""));

    EXPECT_TRUE(xmlDeclaration.isValid());
    EXPECT_EQ(XmlDeclaration::Version_v1_0, xmlDeclaration.version());
    EXPECT_EQ(XmlDeclaration::Encoding_None, xmlDeclaration.encoding());
    EXPECT_EQ(XmlDeclaration::Standalone_No, xmlDeclaration.standalone());

    // Valid version ("v1.0") and invalid standalone
    xmlDeclaration = XmlDeclaration::fromPiData(
                         Utf8::toUnicodeString("version=\"1.0\" standalone=\"Yes\""));

    EXPECT_FALSE(xmlDeclaration.isValid());
    EXPECT_EQ(XmlDeclaration::Version_v1_0, xmlDeclaration.version());
    EXPECT_EQ(XmlDeclaration::Encoding_None, xmlDeclaration.encoding());
    EXPECT_EQ(XmlDeclaration::Standalone_Invalid, xmlDeclaration.standalone());

    // Valid version ("v1.0") and invalid standalone
    xmlDeclaration = XmlDeclaration::fromPiData(
                         Utf8::toUnicodeString("version=\"1.0\" standalone=\"No\""));

    EXPECT_FALSE(xmlDeclaration.isValid());
    EXPECT_EQ(XmlDeclaration::Version_v1_0, xmlDeclaration.version());
    EXPECT_EQ(XmlDeclaration::Encoding_None, xmlDeclaration.encoding());
    EXPECT_EQ(XmlDeclaration::Standalone_Invalid, xmlDeclaration.standalone());

    // Invalid version and valid standalone ("no")
    xmlDeclaration = XmlDeclaration::fromPiData(
                         Utf8::toUnicodeString("version=\"1.1\" standalone=\"no\""));

    EXPECT_FALSE(xmlDeclaration.isValid());
    EXPECT_EQ(XmlDeclaration::Version_Invalid, xmlDeclaration.version());
    EXPECT_EQ(XmlDeclaration::Encoding_None, xmlDeclaration.encoding());
    EXPECT_EQ(XmlDeclaration::Standalone_No, xmlDeclaration.standalone());

    // Invalid version and standalone
    xmlDeclaration = XmlDeclaration::fromPiData(
                         Utf8::toUnicodeString("version=\"1.1\" standalone=\"Yes\""));

    EXPECT_FALSE(xmlDeclaration.isValid());
    EXPECT_EQ(XmlDeclaration::Version_Invalid, xmlDeclaration.version());
    EXPECT_EQ(XmlDeclaration::Encoding_None, xmlDeclaration.encoding());
    EXPECT_EQ(XmlDeclaration::Standalone_Invalid, xmlDeclaration.standalone());

    // Three parameters ****************************************************************************
    // Valid version ("v1.0"), valid encoding ("UTF-8") and valid standalone ("yes")
    xmlDeclaration = XmlDeclaration::fromPiData(
                         Utf8::toUnicodeString("version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\""));

    EXPECT_TRUE(xmlDeclaration.isValid());
    EXPECT_EQ(XmlDeclaration::Version_v1_0, xmlDeclaration.version());
    EXPECT_EQ(XmlDeclaration::Encoding_Utf8, xmlDeclaration.encoding());
    EXPECT_EQ(XmlDeclaration::Standalone_Yes, xmlDeclaration.standalone());

    // Invalid version, valid encoding ("UTF-8") and valid standalone ("yes")
    xmlDeclaration = XmlDeclaration::fromPiData(
                         Utf8::toUnicodeString("version=\"1.1\" encoding=\"UTF-8\" standalone=\"yes\""));

    EXPECT_FALSE(xmlDeclaration.isValid());
    EXPECT_EQ(XmlDeclaration::Version_Invalid, xmlDeclaration.version());
    EXPECT_EQ(XmlDeclaration::Encoding_Utf8, xmlDeclaration.encoding());
    EXPECT_EQ(XmlDeclaration::Standalone_Yes, xmlDeclaration.standalone());

    // Valid version ("v1.0"), invalid encoding and valid standalone ("yes)
    xmlDeclaration = XmlDeclaration::fromPiData(
                         Utf8::toUnicodeString("version=\"1.0\" encoding=\"UTF-\" standalone=\"yes\""));

    EXPECT_FALSE(xmlDeclaration.isValid());
    EXPECT_EQ(XmlDeclaration::Version_v1_0, xmlDeclaration.version());
    EXPECT_EQ(XmlDeclaration::Encoding_Invalid, xmlDeclaration.encoding());
    EXPECT_EQ(XmlDeclaration::Standalone_Yes, xmlDeclaration.standalone());

    // Valid version ("v1.0"), valid encoding ("UTF-8") and invalid standalone
    xmlDeclaration = XmlDeclaration::fromPiData(
                         Utf8::toUnicodeString("version=\"1.0\" encoding=\"UTF-8\" standalone=\"Yes\""));

    EXPECT_FALSE(xmlDeclaration.isValid());
    EXPECT_EQ(XmlDeclaration::Version_v1_0, xmlDeclaration.version());
    EXPECT_EQ(XmlDeclaration::Encoding_Utf8, xmlDeclaration.encoding());
    EXPECT_EQ(XmlDeclaration::Standalone_Invalid, xmlDeclaration.standalone());
}

TEST(EmbeddedStAX_Common_XmlDeclaration, FromPiDataWithTrailingGarbageTest)
{
    XmlDeclaration xmlDeclaration;

    // One parameter *******************************************************************************
    // Valid version ("v1.0")
    xmlDeclaration = XmlDeclaration::fromPiData(Utf8::toUnicodeString("version='1.0' a"));

    EXPECT_FALSE(xmlDeclaration.isValid());
    EXPECT_EQ(XmlDeclaration::Version_None, xmlDeclaration.version());
    EXPECT_EQ(XmlDeclaration::Encoding_None, xmlDeclaration.encoding());
    EXPECT_EQ(XmlDeclaration::Standalone_None, xmlDeclaration.standalone());

    // Two parameters ******************************************************************************
    // Valid version ("v1.0") and encoding ("UTF-8")
    xmlDeclaration = XmlDeclaration::fromPiData(
                         Utf8::toUnicodeString("version='1.0' encoding='UTF-8' a"));

    EXPECT_FALSE(xmlDeclaration.isValid());
    EXPECT_EQ(XmlDeclaration::Version_None, xmlDeclaration.version());
    EXPECT_EQ(XmlDeclaration::Encoding_None, xmlDeclaration.encoding());
    EXPECT_EQ(XmlDeclaration::Standalone_None, xmlDeclaration.standalone());

    // Valid version ("v1.0") and standalone ("yes")
    xmlDeclaration = XmlDeclaration::fromPiData(
                         Utf8::toUnicodeString("version='1.0' standalone='yes' a"));

    EXPECT_FALSE(xmlDeclaration.isValid());
    EXPECT_EQ(XmlDeclaration::Version_None, xmlDeclaration.version());
    EXPECT_EQ(XmlDeclaration::Encoding_None, xmlDeclaration.encoding());
    EXPECT_EQ(XmlDeclaration::Standalone_None, xmlDeclaration.standalone());

    // Three parameters ****************************************************************************
    // Valid version ("v1.0"), valid encoding ("UTF-8") and valid standalone ("yes")
    xmlDeclaration = XmlDeclaration::fromPiData(
                         Utf8::toUnicodeString("version='1.0' encoding='UTF-8' standalone='yes' a"));

    EXPECT_FALSE(xmlDeclaration.isValid());
    EXPECT_EQ(XmlDeclaration::Version_None, xmlDeclaration.version());
    EXPECT_EQ(XmlDeclaration::Encoding_None, xmlDeclaration.encoding());
    EXPECT_EQ(XmlDeclaration::Standalone_None, xmlDeclaration.standalone());
}
