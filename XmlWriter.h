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

#ifndef MINISAXCPP_XMLWRITER_H
#define MINISAXCPP_XMLWRITER_H

#include "Common.h"
#include <list>

namespace MiniSaxCpp
{
/**
 * XML Writer class can be used to create a XML document
 */
class XmlWriter
{
public:
    // Public API
    XmlWriter();

    void clearDocument();
    std::string getXmlString() const;

    bool setXmlDeclaration();
    bool setDocumentType(const std::string &documentType);
    bool addComment(const std::string &commentText);
    bool addProcessingInstruction(const std::string &piTarget, const std::string &piValue);

    bool startRootElement(const std::string &rootElementName);
    bool startChildElement(const std::string &elementName);
    bool addAttribute(const std::string &name,
                      const std::string &value,
                      const Common::QuotationMark quotationMark = Common::QuotationMark_Quote);
    bool addTextNode(const std::string &textNode);
    bool addCDataSection(const std::string &cData);
    bool endElement();

private:
    // Private types
    enum State
    {
        State_Empty,
        State_DocumentStarted,
        State_ElementStarted,
        State_InElement,
        State_DocumentEnded
    };

    struct ElementInfo
    {
        ElementInfo(const std::string &name = std::string(), const bool contentEmpty = true)
            : name(name),
              contentEmpty(contentEmpty)
        {
        }

        std::string name;
        bool contentEmpty;
    };

private:
    // Private data
    State m_state;
    bool m_xmlDeclarationSet;
    std::string m_documentType;
    std::list<ElementInfo> m_openedElementList;
    ElementInfo m_currentElementInfo;
    std::list<std::string> m_attributeNameList;
    std::string m_xmlString;
};
}

#endif // MINISAXCPP_XMLWRITER_H
