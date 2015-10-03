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

#ifndef EMBEDDEDSTAX_XMLREADER_TOKENPARSERS_STARTOFELEMENTPARSER_H
#define EMBEDDEDSTAX_XMLREADER_TOKENPARSERS_STARTOFELEMENTPARSER_H

#include <EmbeddedStAX/XmlReader/TokenParsers/NameParser.h>
#include <EmbeddedStAX/XmlReader/TokenParsers/AttributeValueParser.h>
#include <EmbeddedStAX/Common/Attribute.h>
#include <list>

namespace EmbeddedStAX
{
namespace XmlReader
{
/**
 * Start Of Element parser
 */
class StartOfElementParser: public AbstractTokenParser
{
public:
    // Public API
    StartOfElementParser(ParsingBuffer *parsingBuffer);
    ~StartOfElementParser();

    bool isValid() const;
    Result parse();

    Common::UnicodeString name() const;
    std::list<Common::Attribute> attributeList() const;

private:
    // Private types
    enum State
    {
        State_Idle,
        State_ReadingElementName,
        State_ReadingAttributeName,
        State_ReadingEqualSign,
        State_ReadingAttributeValue,
        State_ReadingNextAttribute,
        State_ReadingEndOfEmptyElement,
        State_Finished,
        State_Error
    };

private:
    // Private API
    State executeStateReadingElementName();
    State executeStateReadingAttributeName();
    State executeStateReadingEqualSign();
    State executeStateReadingAttributeValue();
    State executeStateReadingNextAttribute();
    State executeStateReadingEndOfEmptyElement();

private:
    // Private data
    State m_state;
    NameParser *m_nameParser;
    AttributeValueParser *m_attributeValueParser;
    Common::UnicodeString m_elementName;
    Common::UnicodeString m_attributeName;
    std::list<Common::Attribute> m_attributeList;
};
}
}

#endif // EMBEDDEDSTAX_XMLREADER_TOKENPARSERS_STARTOFELEMENTPARSER_H
