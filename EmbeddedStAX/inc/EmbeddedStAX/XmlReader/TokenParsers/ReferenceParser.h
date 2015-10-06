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

#ifndef EMBEDDEDSTAX_XMLREADER_TOKENPARSERS_REFERENCEPARSER_H
#define EMBEDDEDSTAX_XMLREADER_TOKENPARSERS_REFERENCEPARSER_H

#include <EmbeddedStAX/XmlReader/TokenParsers/NameParser.h>

namespace EmbeddedStAX
{
namespace XmlReader
{
/**
 * Reference parser
 */
class ReferenceParser: public AbstractTokenParser
{
public:
    // Public API
    ReferenceParser();
    ~ReferenceParser();

    Common::UnicodeString value() const;

    virtual Result parse();

private:
    // Private types
    enum State
    {
        State_ReadingStartOfReference,
        State_ReadingReferenceType,
        State_ReadingEntityReferenceName,
        State_ReadingCharacterReferenceType,
        State_ReadingCharacterReferenceDecimal,
        State_ReadingCharacterReferenceHexadecimal,
        State_Finished,
        State_Error
    };

private:
    // Private API
    virtual bool initializeAdditionalData();

    State executeStateReadingStartOfReference();
    State executeStateReadingReferenceType();
    State executeStateReadingEntityReferenceName();
    State executeStateReadingCharacterReferenceType();
    State executeStateReadingCharacterReferenceDecimal();
    State executeStateReadingCharacterReferenceHexadecimal();

private:
    // Private data
    State m_state;
    NameParser m_nameParser;
    Common::UnicodeString m_value;
    uint32_t m_charRefValue;
};
}
}

#endif // EMBEDDEDSTAX_XMLREADER_TOKENPARSERS_REFERENCEPARSER_H
