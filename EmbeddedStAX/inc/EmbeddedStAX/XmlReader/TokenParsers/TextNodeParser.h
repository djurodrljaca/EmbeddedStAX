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

#ifndef EMBEDDEDSTAX_XMLREADER_TOKENPARSERS_TEXTNODEPARSER_H
#define EMBEDDEDSTAX_XMLREADER_TOKENPARSERS_TEXTNODEPARSER_H

#include <EmbeddedStAX/XmlReader/TokenParsers/AbstractTokenParser.h>
#include <EmbeddedStAX/XmlReader/TokenParsers/ReferenceParser.h>

namespace EmbeddedStAX
{
namespace XmlReader
{
/**
 * Text node parser
 */
class TextNodeParser: public AbstractTokenParser
{
public:
    // Public API
    TextNodeParser(ParsingBuffer *parsingBuffer);
    ~TextNodeParser();

    bool isValid() const;
    Result parse();
    Common::UnicodeString text() const;

private:
    // Private types
    enum State
    {
        State_Idle,
        State_ReadingText,
        State_ReadingReference,
        State_Finished,
        State_Error
    };

private:
    // Private API
    State executeStateReadingText();
    State executeStateReadingReference();

private:
    // Private data
    State m_state;
    ReferenceParser *m_referenceParser;
    Common::UnicodeString m_text;
};
}
}

#endif // EMBEDDEDSTAX_XMLREADER_TOKENPARSERS_TEXTNODEPARSER_H