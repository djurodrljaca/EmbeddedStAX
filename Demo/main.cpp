/*
 * This is free and unencumbered software released into the public domain.
 *
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 *
 * In jurisdictions that recognize copyright laws, the author or authors
 * of this software dedicate any and all copyright interest in the
 * software to the public domain. We make this dedication for the benefit
 * of the public at large and to the detriment of our heirs and
 * successors. We intend this dedication to be an overt act of
 * relinquishment in perpetuity of all present and future rights to this
 * software under copyright law.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * For more information, please refer to <http://unlicense.org>
 */

#include <iostream>
#include <EmbeddedStAX/XmlReader/XmlReader.h>
#include <EmbeddedStAX/XmlWriter/XmlWriter.h>

using namespace EmbeddedStAX;

std::string executeXmlWriter();
void executeXmlReader(const std::string &xmlString);

int main(int argc, char **argv)
{
    std::string xmlString = "<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>\n"
                            "<?pitarget      pidata       ?>\n"
                            "<!--comment text-->\n"
                            "<!DOCTYPE root >\n"
                            "<?pitarget2      pidata2       ?>\n"
                            "<!--comment text2-->\n"
                            "<root a1='asd' a2=\"fgh; 'amp' entity reference ('&amp;')\">"
                            "asd; 'amp' entity reference ('&amp;')<![CDATA[asdasdasdasdasd]]>\n"
                            "  <child1 /> some text\n"
                            "  <child2 a='b'>more text</child2>\n"
                            "  <child3><child4>asdfgh</child4        ></child3>\n"
                            "  <child5/>some text\n"
                            "</root>\n"
                            "<?pitarget      pidata       ?>\n"
                            "<!--comment text-->";

    executeXmlReader(xmlString);

    xmlString.clear();
    xmlString = executeXmlWriter();

    std::cout << "XML String:" << std::endl << xmlString << std::endl;

    executeXmlReader(xmlString);

    return 0;
}

std::string executeXmlWriter()
{
    std::string xmlString;
    XmlWriter::XmlWriter xmlWriter;

    bool success = xmlWriter.writeXmlDeclaration();

    if (success)
    {
        success = xmlWriter.writeProcessingInstruction(
                      Common::ProcessingInstruction(
                          Common::Utf8::toUnicodeString("pitarget"),
                          Common::Utf8::toUnicodeString("     pidata       ")));
    }

    if (success)
    {
        success = xmlWriter.writeComment(Common::Utf8::toUnicodeString("comment text"));
    }

    if (success)
    {
        success = xmlWriter.writeProcessingInstruction(
                      Common::ProcessingInstruction(
                          Common::Utf8::toUnicodeString("pitarget2"),
                          Common::Utf8::toUnicodeString("     pidata2       ")));
    }

    if (success)
    {
        success = xmlWriter.writeComment(Common::Utf8::toUnicodeString("comment text2"));
    }

    if (success)
    {
        Common::AttributeList attributeList;
        attributeList.add(Common::Attribute(Common::Utf8::toUnicodeString("a1"),
                                            Common::Utf8::toUnicodeString("asd"),
                                            Common::QuotationMark_Apostrophe));
        attributeList.add(
                    Common::Attribute(
                        Common::Utf8::toUnicodeString("a2"),
                        Common::Utf8::toUnicodeString("fgh; 'amp' entity reference ('&amp;')")));

        success = xmlWriter.writeStartOfElement(Common::Utf8::toUnicodeString("root"),
                                                attributeList);
    }

    if (success)
    {
        success = xmlWriter.writeTextNode(
                      Common::Utf8::toUnicodeString("asd; 'amp' entity reference ('&amp;')"));
    }

    if (success)
    {
        success = xmlWriter.writeCDataSection(
                      Common::Utf8::toUnicodeString("asdasdasdasdasd"));
    }

    if (success)
    {
        success = xmlWriter.writeEmptyElement(Common::Utf8::toUnicodeString("child1"));
    }

    if (success)
    {
        success = xmlWriter.writeTextNode(Common::Utf8::toUnicodeString(" some text"));
    }

    if (success)
    {
        Common::AttributeList attributeList;
        attributeList.add(Common::Attribute(Common::Utf8::toUnicodeString("a"),
                                            Common::Utf8::toUnicodeString("b"),
                                            Common::QuotationMark_Apostrophe));

        success = xmlWriter.writeStartOfElement(Common::Utf8::toUnicodeString("child2"),
                                                attributeList);
    }

    if (success)
    {
        success = xmlWriter.writeTextNode(Common::Utf8::toUnicodeString("more text"));
    }

    if (success)
    {
        success = xmlWriter.writeEndOfElement();
    }

    if (success)
    {
        success = xmlWriter.writeStartOfElement(Common::Utf8::toUnicodeString("child3"));
    }

    if (success)
    {
        success = xmlWriter.writeStartOfElement(Common::Utf8::toUnicodeString("child4"));
    }

    if (success)
    {
        success = xmlWriter.writeTextNode(Common::Utf8::toUnicodeString("asdfgh"));
    }

    if (success)
    {
        success = xmlWriter.writeEndOfElement();
    }

    if (success)
    {
        success = xmlWriter.writeEndOfElement();
    }

    if (success)
    {
        success = xmlWriter.writeEmptyElement(Common::Utf8::toUnicodeString("child5"));
    }

    if (success)
    {
        success = xmlWriter.writeTextNode(Common::Utf8::toUnicodeString(" some text"));
    }

    if (success)
    {
        success = xmlWriter.writeEndOfElement();
    }

    if (success)
    {
        success = xmlWriter.writeProcessingInstruction(
                      Common::ProcessingInstruction(
                          Common::Utf8::toUnicodeString("pitarget"),
                          Common::Utf8::toUnicodeString("     pidata       ")));
    }

    if (success)
    {
        success = xmlWriter.writeComment(Common::Utf8::toUnicodeString("comment text"));
    }

    if (success)
    {
        xmlString = Common::Utf8::toUtf8(xmlWriter.xmlString());
    }

    return xmlString;
}

void executeXmlReader(const std::string &xmlString)
{
    XmlReader::XmlReader xmlReader;

    const size_t bytesWritten = xmlReader.writeData(xmlString);
    bool success = false;

    if (bytesWritten == xmlString.size())
    {
        success = true;
    }

    std::cout << "Write data: success = " << success << ", bytesWritten = " << bytesWritten
              << ", data size = " << xmlString.size() << std::endl;

    while (success)
    {
        const XmlReader::XmlReader::ParsingResult result = xmlReader.parse();

        switch (result)
        {
            case XmlReader::XmlReader::ParsingResult_XmlDeclaration:
            {
                const Common::XmlDeclaration xmlDeclaration = xmlReader.xmlDeclaration();

                std::cout << "XML declaration: version = " << xmlDeclaration.version()
                          << ", encoding = " << xmlDeclaration.encoding()
                          << ", standalone = " << xmlDeclaration.standalone() << std::endl;
                break;
            }

            case XmlReader::XmlReader::ParsingResult_ProcessingInstruction:
            {
                const Common::ProcessingInstruction processingInstruction =
                        xmlReader.processingInstruction();
                const std::string name = Common::Utf8::toUtf8(processingInstruction.piTarget());
                const std::string data = Common::Utf8::toUtf8(processingInstruction.piData());

                std::cout << "Processing Instruction: name = " << name << ", data = " << data
                          << std::endl;
                break;
            }

            case XmlReader::XmlReader::ParsingResult_Comment:
            {
                const std::string text = Common::Utf8::toUtf8(xmlReader.text());
                std::cout << "Comment: text = " << text << std::endl;
                break;
            }

            case XmlReader::XmlReader::ParsingResult_DocumentType:
            {
                const Common::DocumentType documentType = xmlReader.documentType();
                const std::string name = Common::Utf8::toUtf8(documentType.name());

                std::cout << "Document type: name = " << name << std::endl;
                break;
            }

            case XmlReader::XmlReader::ParsingResult_StartOfElement:
            {
                const std::string name = Common::Utf8::toUtf8(xmlReader.name());

                std::cout << "Start of element: name = " << name << std::endl;

                const Common::AttributeList attributeList = xmlReader.attributeList();

                for (Common::AttributeList::Iterator it = attributeList.first();
                     it.isValid();
                     it.next())
                {
                    const Common::Attribute *attribute = it.value();

                    if (attribute != NULL)
                    {
                        const std::string attributeName = Common::Utf8::toUtf8(attribute->name());
                        const std::string attributeValue = Common::Utf8::toUtf8(attribute->value());

                        std::cout << "    Attribute: name = " << attributeName << " value = "
                                  << attributeValue << std::endl;
                    }
                    else
                    {
                        std::cout << "Error: null pointer found in AttributeList iterator"
                                  << std::endl;
                    }
                }
                break;
            }

            case XmlReader::XmlReader::ParsingResult_TextNode:
            {
                const std::string text = Common::Utf8::toUtf8(xmlReader.text());
                std::cout << "Text Node: text = " << text << std::endl;
                break;
            }

            case XmlReader::XmlReader::ParsingResult_CData:
            {
                const std::string text = Common::Utf8::toUtf8(xmlReader.text());
                std::cout << "CDATA: text = " << text << std::endl;
                break;
            }

            case XmlReader::XmlReader::ParsingResult_EndOfElement:
            {
                const std::string name = Common::Utf8::toUtf8(xmlReader.name());

                std::cout << "End of element: name = " << name << std::endl;
                break;
            }

            default:
            {
                std::cout << "Default: parsing result = " << result << std::endl;
                success = false;
                break;
            }
        }
    }
}
