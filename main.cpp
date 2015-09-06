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

using namespace MiniSaxCpp;

int main(int argc, char **argv)
{
    XmlWriter xmlWriter;

    bool success = xmlWriter.setXmlDeclaration();

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

    if (success)
    {
        std::cout << "XML document:" << std::endl << xmlWriter.getXmlString() << std::endl;
    }

    return 0;
}
