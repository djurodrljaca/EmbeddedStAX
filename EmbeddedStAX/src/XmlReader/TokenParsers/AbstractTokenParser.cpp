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
 * \param parserType    Token parser type
 */
AbstractTokenParser::AbstractTokenParser(const ParserType parserType)
    : m_initialized(false),
      m_parsingBuffer(NULL),
      m_option(Option_None),
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
 * Get parsing option
 *
 * \return Parsing option
 */
AbstractTokenParser::Option AbstractTokenParser::option() const
{
    return m_option;
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
 * Initialize parser
 *
 * \param parsingBuffer Pointer to a parsing buffer
 * \param option        Parsing option
 *
 * \retval true     Success
 * \retval false    Error
 *
 * \note This should be called before parsing of a new token is started.
 */
bool AbstractTokenParser::initialize(ParsingBuffer *parsingBuffer,
                                     const AbstractTokenParser::Option option)
{
    // First make sure that initialization flag is cleared
    m_initialized = false;

    // Initialize parser
    bool success = false;

    if (parsingBuffer != NULL)
    {
        // First the parsing buffer must be initiaized and then everything else
        m_parsingBuffer = parsingBuffer;
        m_tokenType = TokenType_None;
        m_terminationChar = 0U;

        success = setOption(option);
    }

    if (success)
    {
        // It is important that everyting else is initialized up to this point
        success = initializeAdditionalData();
    }

    // Update initialized flag
    m_initialized = success;

    return success;
}

/**
 * Check if parser is initialized
 *
 * \retval true     Initialized
 * \retval false    Uninitialized
 */
bool AbstractTokenParser::isInitialized() const
{
    return m_initialized;
}

/**
 * Set parsing option
 *
 * \param option    Parsing option
 *
 * \retval true     Parsing option set
 * \retval false    Parsing option not set
 *
 * \note It has to be overriden if only specific options are allowed in a derived class!
 */
bool AbstractTokenParser::setOption(const Option option)
{
    m_option = option;
    return true;
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

/**
 * Initialize parser's additional data
 *
 * \retval true     Success
 * \retval false    Error
 *
 * \note It has to be overriden in a derived class if it contains any additional data!
 */
bool AbstractTokenParser::initializeAdditionalData()
{
    return true;
}
