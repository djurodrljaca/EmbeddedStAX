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
#include "XmlWriter.h"
#include "XmlReader.h"

using namespace MiniSaxCpp;

int main(int argc, char **argv)
{
    XmlWriter xmlWriter;

    bool success = xmlWriter.setXmlDeclaration();

    if (success)
    {
        success = xmlWriter.addProcessingInstruction("piTarget", "PI's value");
    }

    if (success)
    {
        success = xmlWriter.addComment("comment text");
    }

    if (success)
    {
        success = xmlWriter.addProcessingInstruction("piTarget", "PI's value");
    }

    if (success)
    {
        success = xmlWriter.setDocumentType("root");
    }

    if (success)
    {
        success = xmlWriter.startRootElement("root");
    }

    if (success)
    {
        success = xmlWriter.addAttribute("attribute1", "", Common::QuotationMark_Quote);
    }

    if (success)
    {
        success = xmlWriter.addAttribute("attribute2", "value2", Common::QuotationMark_Apostrophe);
    }

    if (success)
    {
        const std::string escapedString =
                Common::escapeAttributeValueString("value1 < value2 & value3 \" ' <&\"' value4",
                                                   Common::QuotationMark_Quote);
        success = xmlWriter.addAttribute("attribute3", escapedString, Common::QuotationMark_Quote);
    }

    if (success)
    {
        const std::string escapedString =
                Common::escapeAttributeValueString("value1 < value2 & value3 \" ' <&\"' value4",
                                                   Common::QuotationMark_Apostrophe);
        success = xmlWriter.addAttribute("attribute4",
                                         escapedString,
                                         Common::QuotationMark_Apostrophe);
    }

    if (success)
    {
        success = xmlWriter.addComment("comment text");
    }

    if (success)
    {
        success = xmlWriter.startChildElement("child1");
    }

    if (success)
    {
        success = xmlWriter.addTextNode("some text");
    }

    if (success)
    {
        success = xmlWriter.addCDataSection("some cdata text");
    }

    if (success)
    {
        success = xmlWriter.endElement();
    }

    if (success)
    {
        success = xmlWriter.startChildElement("child2");
    }

    if (success)
    {
        success = xmlWriter.addAttribute("attribute2", "value2", Common::QuotationMark_Apostrophe);
    }

    if (success)
    {
        success = xmlWriter.endElement();
    }

    if (success)
    {
        success = xmlWriter.startChildElement("child3");
    }

    if (success)
    {
        success = xmlWriter.addAttribute("attribute2", "value2", Common::QuotationMark_Apostrophe);
    }

    if (success)
    {
        success = xmlWriter.addProcessingInstruction("piTarget", "PI's value");
    }

    if (success)
    {
        success = xmlWriter.startChildElement("child4");
    }

    if (success)
    {
        const std::string escapedString =
                Common::escapeTextNodeString("value1 < value2 & value3 \" ' <&\"' value4");
        success = xmlWriter.addTextNode(escapedString);
    }

    if (success)
    {
        success = xmlWriter.endElement();
    }

    if (success)
    {
        success = xmlWriter.endElement();
    }

    if (success)
    {
        success = xmlWriter.endElement();
    }

    if (success)
    {
        success = xmlWriter.addProcessingInstruction("piTarget", "PI's value");
    }

    if (success)
    {
        success = xmlWriter.addComment("comment text");
    }

    //----------------------------------------------------------------------------------------------

    if (success)
    {
        const std::string xmlString = xmlWriter.getXmlString();

        std::cout << "XML document size: " << xmlString.size() << std::endl;
        std::cout << "XML document: " << std::endl << xmlString << std::endl;

        const size_t bufferSize = 100U;

        XmlReader xmlReader(bufferSize);
        size_t position = 0U;

        while (success && (position < xmlString.size()))
        {
            XmlReader::ParsingResult result = xmlReader.parse();

            switch (result)
            {
                case XmlReader::ParsingResult_NeedMoreData:
                {
                    success = xmlReader.writeData(xmlString.at(position));

                    if (success)
                    {
                        position++;
                    }
                    break;
                }

                case XmlReader::ParsingResult_XmlDeclaration:
                {
                    std::cout << "XML declaration: " << xmlReader.getName() << ":"
                              << xmlReader.getValue() << std::endl;
                    break;
                }

                case XmlReader::ParsingResult_ProcessingInstruction:
                {
                    std::cout << "XML PI: " << xmlReader.getName() << ":" << xmlReader.getValue()
                              << std::endl;
                    break;
                }

                case XmlReader::ParsingResult_DocumentType:
                {
                    std::cout << "XML DOCTYPE: " << xmlReader.getName() << std::endl;
                    break;
                }

                case XmlReader::ParsingResult_Comment:
                {
                    std::cout << "XML Comment: " << xmlReader.getValue() << std::endl;
                    break;
                }

                case XmlReader::ParsingResult_StartOfRootElement:
                {
                    std::cout << "XML Start Of Root Element: " << xmlReader.getName() << std::endl;
                    break;
                }

                case XmlReader::ParsingResult_StartOfChildElement:
                {
                    std::cout << "XML Start Of Child Element: " << xmlReader.getName() << std::endl;
                    break;
                }

                case XmlReader::ParsingResult_ElementAttribute:
                {
                    std::cout << "XML Element Attribute: " << xmlReader.getName() << " = "
                              << xmlReader.getValue() << std::endl;
                    break;
                }

                case XmlReader::ParsingResult_EndOfRootElement:
                {
                    std::cout << "XML End Of Root Element: " << xmlReader.getName() << std::endl;
                    break;
                }

                case XmlReader::ParsingResult_EndOfChildElement:
                {
                    std::cout << "XML End Of Root Element: " << xmlReader.getName() << std::endl;
                    break;
                }


                default:
                {
                    std::cout << "default: result: " << result << std::endl;
                    success = false;
                    break;
                }
            }
        }
    }

    return 0;
}
