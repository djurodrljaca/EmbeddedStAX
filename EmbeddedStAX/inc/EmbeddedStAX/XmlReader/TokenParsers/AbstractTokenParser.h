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
        ParserType_AttributeValue,
        ParserType_CData,
        ParserType_Comment,
        ParserType_DocumentType,
        ParserType_Name,
        ParserType_ProcessingInstruction,
        ParserType_Reference,
        ParserType_TextNode,
        ParserType_TokenType,
    };

    enum TokenType
    {
        TokenType_None,
        TokenType_CData,
        TokenType_CharacterReference,
        TokenType_Comment,
        TokenType_DocumentType,
        TokenType_EndOfElement,
        TokenType_EmptyElement,
        TokenType_EntityReference,
        TokenType_ProcessingInstruction,
        TokenType_StartOfElement,
        TokenType_TextNode,
        TokenType_Whitespace,
        TokenType_XmlDeclaration,
    };

public:
    // Public API
    AbstractTokenParser(const ParserType parserType);
    virtual ~AbstractTokenParser() = 0;

    Option option() const;
    ParserType parserType() const;
    TokenType tokenType() const;
    uint32_t terminationChar() const;

    bool initialize(ParsingBuffer *parsingBuffer, const Option option = Option_None);
    virtual Result parse() = 0;
    void deinitialize();

protected:
    // Protected API
    bool isInitialized() const;
    virtual bool setOption(const Option option);
    ParsingBuffer *parsingBuffer();
    void setTokenType(const TokenType tokenType);
    void setTerminationChar(const uint32_t uchar);
    virtual bool initializeAdditionalData() = 0;
    virtual void deinitializeAdditionalData() = 0;

private:
    // Private data
    bool m_initialized;
    ParsingBuffer *m_parsingBuffer;
    Option m_option;
    TokenType m_tokenType;
    uint32_t m_terminationChar;
    const ParserType m_parserType;
};
}
}

#endif // EMBEDDEDSTAX_XMLREADER_TOKENPARSERS_ABSTRACTTOKENPARSER_H
