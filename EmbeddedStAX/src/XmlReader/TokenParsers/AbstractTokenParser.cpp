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

#include <EmbeddedStAX/XmlReader/TokenParsers/AbstractTokenParser.h>

using namespace EmbeddedStAX::XmlReader;

/**
 * Constructor
 *
 * \param parsingBuffer Pointer to a parsing buffer
 * \param option        Parsing option
 * \param parserType    Token parser type
 */
AbstractTokenParser::AbstractTokenParser(ParsingBuffer *parsingBuffer,
                                         const Option option,
                                         const ParserType parserType)
    : m_parsingBuffer(parsingBuffer),
      m_option(option),
      m_tokenType(TokenType_None),
      m_terminationChar(0U),
      m_parserType(parserType)
{
}

/**
 * Destructor
 */
AbstractTokenParser::~AbstractTokenParser()
{
    m_parsingBuffer = NULL;
}

/**
 * Check if parser is valid
 *
 * \retval true     Valid
 * \retval false    Invalid
 */
bool AbstractTokenParser::isValid() const
{
    bool valid = false;

    if ((m_parsingBuffer != NULL) &&
        (m_parserType != ParserType_Invalid))
    {
        valid = true;
    }

    return valid;
}

/**
 * Get parsing option
 *
 * \return Parsing option
 */
AbstractTokenParser::Option AbstractTokenParser::option() const
{
    return m_option;
}

/**
 * Set parsing option
 *
 * \param option    Parsing option
 *
 * \retval false    Parsing option not set
 *
 * \note It has to be overriden if needed in a derived class!
 */
bool AbstractTokenParser::setOption(const AbstractTokenParser::Option option)
{
    return false;
}

/**
 * Get token parser type
 *
 * \return Token parser type
 */
AbstractTokenParser::ParserType AbstractTokenParser::parserType() const
{
    return m_parserType;
}

/**
 * Get type of the token that was found during parsing
 *
 * \return Token type
 */
AbstractTokenParser::TokenType AbstractTokenParser::tokenType() const
{
    return m_tokenType;
}

/**
 * Get termination character
 *
 * \return Termination character
 */
uint32_t AbstractTokenParser::terminationChar() const
{
    return m_terminationChar;
}

/**
 * Get parsing buffer
 *
 * \return Parsing buffer
 */
ParsingBuffer *AbstractTokenParser::parsingBuffer()
{
    return m_parsingBuffer;
}

/**
 * Set token type
 *
 * \param tokenType New token type
 */
void AbstractTokenParser::setTokenType(const AbstractTokenParser::TokenType tokenType)
{
    m_tokenType = tokenType;
}

/**
 * Set termination character
 *
 * \param uchar Termination character
 */
void AbstractTokenParser::setTerminationChar(const uint32_t uchar)
{
    m_terminationChar = uchar;
}
