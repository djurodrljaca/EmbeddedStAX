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

#ifndef EMBEDDEDSTAX_XMLREADER_TOKENPARSERS_ABSTRACTTOKENPARSER_H
#define EMBEDDEDSTAX_XMLREADER_TOKENPARSERS_ABSTRACTTOKENPARSER_H

#include <EmbeddedStAX/XmlReader/ParsingBuffer.h>

namespace EmbeddedStAX
{
namespace XmlReader
{
/**
 * Abstract token parser
 */
class AbstractTokenParser
{
public:
    // Public types
    enum Result
    {
        Result_NeedMoreData,
        Result_Success,
        Result_Error
    };

    enum Option
    {
        Option_None,
        Option_Synchronization,
        Option_IgnoreLeadingWhitespace
    };

    enum ParserType
    {
        ParserType_Invalid,
        ParserType_TokenType
    };

    enum TokenType
    {
        TokenType_None,
        TokenType_Whitespace,
        TokenType_ProcessingInstruction,
        TokenType_XmlDeclaration,
        TokenType_DocumentType,
        TokenType_Comment,
        TokenType_CData,
        TokenType_StartOfElement,
        TokenType_EndOfElement,
        TokenType_TextNode,
    };

public:
    // Public API
    AbstractTokenParser(ParsingBuffer *parsingBuffer,
                        const Option parsingOption,
                        const ParserType parserType);
    virtual ~AbstractTokenParser() = 0;

    virtual bool isValid() const;
    Option option() const;
    virtual bool setOption(const Option parsingOption);
    ParserType parserType() const;
    TokenType tokenFound() const;
    uint32_t terminationChar() const;
    virtual Result parse() = 0;

protected:
    // Protected data
    ParsingBuffer *m_parsingBuffer;
    Option m_option;
    TokenType m_tokenFound;
    uint32_t m_terminationChar;

private:
    // Private data
    const ParserType m_parserType;
};
}
}

#endif // EMBEDDEDSTAX_XMLREADER_TOKENPARSERS_ABSTRACTTOKENPARSER_H
