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

#ifndef EMBEDDEDSTAX_XMLWRITER_H
#define EMBEDDEDSTAX_XMLWRITER_H

#include <EmbeddedStAX/Common/Attribute.h>
#include <EmbeddedStAX/Common/ProcessingInstruction.h>
#include <EmbeddedStAX/Common/Utf.h>
#include <list>

namespace EmbeddedStAX
{
namespace XmlWriter
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
    Common::UnicodeString xmlString() const;

    bool writeXmlDeclaration();
    bool writeDocumentType(const Common::UnicodeString &documentType);
    bool writeComment(const Common::UnicodeString &commentText);
    bool writeProcessingInstruction(const Common::ProcessingInstruction &pi);
    bool writeEmptyElement(const Common::UnicodeString &elementName,
                           const Common::AttributeList &attributeList = Common::AttributeList());
    bool writeStartOfElement(const Common::UnicodeString &elementName,
                             const Common::AttributeList &attributeList = Common::AttributeList());
    bool writeTextNode(const Common::UnicodeString &textNode);
    bool writeCDataSection(const Common::UnicodeString &cdata);
    bool writeEndOfElement();

private:
    // Private API
    bool writeAttributeList(const Common::AttributeList &attributeList);

private:
    // Private types
    enum State
    {
        State_Empty,
        State_DocumentStarted,
        State_Element,
        State_DocumentEnded,
        State_Error
    };

private:
    // Private data
    State m_state;
    Common::UnicodeString m_documentType;
    std::list<Common::UnicodeString> m_openedElementList; // TODO: replace with custom list object?
    Common::UnicodeString m_xmlString;
};
}
}

#endif // EMBEDDEDSTAX_XMLWRITER_H
