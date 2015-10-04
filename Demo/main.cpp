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

using namespace EmbeddedStAX;

int main(int argc, char **argv)
{
    XmlReader::XmlReader xmlReader;

    const std::string data = "<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>"
                             "<?pitarget      pidata       ?>"
                             "<!--comment text-->"
                             "<!DOCTYPE root >"
                             "<?pitarget2      pidata2       ?>"
                             "<!--comment text2-->"
                             "<root a1='asd' a2=\"fgh; 'amp' entity reference ('&amp;')\">asd; 'amp' entity reference ('&amp;')"
                             "  <child1 /> some text"
                             "  <child2 a='b'>more text</child2>"
                             "  <child3><child4>asdfgh</child4></child3>"
                             "</root>";
    const size_t bytesWritten = xmlReader.writeData(data);
    bool success = false;

    if (bytesWritten == data.size())
    {
        success = true;
    }

    std::cout << "Write data: success = " << success << ", bytesWritten = " << bytesWritten
              << ", data size = " << data.size() << std::endl;

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

                std::cout << "Processing Instruction: name = " << processingInstruction.piTarget()
                          << ", data = " << processingInstruction.piData() << std::endl;
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

                std::cout << "Document type: name = " << documentType.name() << std::endl;
                break;
            }

            case XmlReader::XmlReader::ParsingResult_StartOfElement:
            {
                const std::string name = Common::Utf8::toUtf8(xmlReader.name());

                std::cout << "Start of element: name = " << name << std::endl;

                const std::list<Common::Attribute> attributeList = xmlReader.attributeList();

                for (std::list<Common::Attribute>::const_iterator it = attributeList.begin();
                     it != attributeList.end();
                     it++)
                {
                    const Common::Attribute &attribute = *it;
                    const std::string attributeName = Common::Utf8::toUtf8(attribute.name());
                    const std::string attributeValue = Common::Utf8::toUtf8(attribute.value());

                    std::cout << "    Attribute: name = " << attributeName << " value = "
                              << attributeValue << std::endl;

                }

                break;
            }

            case XmlReader::XmlReader::ParsingResult_TextNode:
            {
                const std::string text = Common::Utf8::toUtf8(xmlReader.text());
                std::cout << "Text Node: text = " << text << std::endl;
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

    return 0;
}
