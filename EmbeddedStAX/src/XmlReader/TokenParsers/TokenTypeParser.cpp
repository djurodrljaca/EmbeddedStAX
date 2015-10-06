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

#include <EmbeddedStAX/XmlReader/TokenParsers/TokenTypeParser.h>
#include <EmbeddedStAX/XmlValidator/Common.h>
#include <EmbeddedStAX/XmlValidator/Name.h>

using namespace EmbeddedStAX::XmlReader;

/**
 * Constructor
 *
 * \param parsingBuffer Pointer to a parsing buffer
 * \param option        Parsing option
 */
TokenTypeParser::TokenTypeParser(ParsingBuffer *parsingBuffer, Option option)
    : AbstractTokenParser(parsingBuffer, option, ParserType_TokenType),
      m_state(State_WaitingForStartOfToken)
{
}

/**
 * Destructor
 */
TokenTypeParser::~TokenTypeParser()
{
}

/**
 * Check if parser is valid
 *
 * \retval true     Valid
 * \retval false    Invalid
 */
bool TokenTypeParser::isValid() const
{
    bool valid = AbstractTokenParser::isValid();

    if (valid)
    {
        switch (option())
        {
            case Option_None:
            case Option_IgnoreLeadingWhitespace:
            case Option_Synchronization:
            {
                // Valid option
                break;
            }

            default:
            {
                // Invalid option
                valid = false;
                break;
            }
        }
    }

    return valid;
}

/**
 * Set parsing option
 *
 * \param parsingOption New parsing option
 *
 * \retval true     Parsing option set
 * \retval false    Parsing option not set
 */
bool TokenTypeParser::setOption(const AbstractTokenParser::Option parsingOption)
{
    bool success = false;

    switch (parsingOption)
    {
        case Option_None:
        case Option_IgnoreLeadingWhitespace:
        case Option_Synchronization:
        {
            // Valid option
            setOption(parsingOption);
            success = true;
            break;
        }

        default:
        {
            // Invalid option
            break;
        }
    }

    return success;
}

/**
 * Parse
 *
 * \retval Result_Success       Success
 * \retval Result_NeedMoreData  More data is needed
 * \retval Result_Error         Error
 */
AbstractTokenParser::Result TokenTypeParser::parse()
{
    Result result = Result_Error;

    if (isValid())
    {
        bool finishParsing = false;

        while (!finishParsing)
        {
            finishParsing = true;
            State nextState = State_Error;

            switch (m_state)
            {
                case State_WaitingForStartOfToken:
                {
                    // Waiting for start of token
                    nextState = executeStateWaitingForStartOfToken();

                    // Check transitions
                    switch (nextState)
                    {
                        case State_WaitingForStartOfToken:
                        {
                            result = Result_NeedMoreData;
                            break;
                        }

                        case State_ReadingTokenType:
                        {
                            // Execute another cycle
                            finishParsing = false;
                            break;
                        }

                        case State_Finished:
                        {
                            result = Result_Success;
                            break;
                        }

                        default:
                        {
                            // Error
                            nextState = State_Error;
                            break;
                        }
                    }
                    break;
                }

                case State_ReadingTokenType:
                {
                    // Reading token type
                    nextState = executeStateReadingTokenType();

                    // Check transitions
                    switch (nextState)
                    {
                        case State_ReadingTokenType:
                        {
                            result = Result_NeedMoreData;
                            break;
                        }

                        case State_ReadingTokenTypeExclamationMark:
                        {
                            // Execute another cycle
                            finishParsing = false;
                            break;
                        }

                        case State_Finished:
                        {
                            result = Result_Success;
                            break;
                        }

                        default:
                        {
                            // Error
                            nextState = State_Error;
                            break;
                        }
                    }
                    break;
                }

                case State_ReadingTokenTypeExclamationMark:
                {
                    // Reading token type that starts with an exclamation mark
                    nextState = executeStateReadingTokenTypeExclamationMark();

                    // Check transitions
                    switch (nextState)
                    {
                        case State_ReadingTokenTypeExclamationMark:
                        {
                            result = Result_NeedMoreData;
                            break;
                        }

                        case State_ReadingTokenTypeDocumentType:
                        case State_ReadingTokenTypeComment:
                        case State_ReadingTokenTypeCData:
                        {
                            // Execute another cycle
                            finishParsing = false;
                            break;
                        }

                        default:
                        {
                            // Error
                            nextState = State_Error;
                            break;
                        }
                    }
                    break;
                }

                case State_ReadingTokenTypeDocumentType:
                {
                    // Reading document type token type
                    nextState = executeStateReadingTokenTypeDocumentType();

                    // Check transitions
                    switch (nextState)
                    {
                        case State_ReadingTokenTypeDocumentType:
                        {
                            result = Result_NeedMoreData;
                            break;
                        }

                        case State_Finished:
                        {
                            result = Result_Success;
                            break;
                        }

                        default:
                        {
                            // Error
                            nextState = State_Error;
                            break;
                        }
                    }
                    break;
                }

                case State_ReadingTokenTypeComment:
                {
                    // Reading comment token type
                    nextState = executeStateReadingTokenTypeComment();

                    // Check transitions
                    switch (nextState)
                    {
                        case State_ReadingTokenTypeComment:
                        {
                            result = Result_NeedMoreData;
                            break;
                        }

                        case State_Finished:
                        {
                            result = Result_Success;
                            break;
                        }

                        default:
                        {
                            // Error
                            nextState = State_Error;
                            break;
                        }
                    }
                    break;
                }

                case State_ReadingTokenTypeCData:
                {
                    // Reading CDATA token type
                    nextState = executeStateReadingTokenTypeCData();

                    // Check transitions
                    switch (nextState)
                    {
                        case State_ReadingTokenTypeCData:
                        {
                            result = Result_NeedMoreData;
                            break;
                        }

                        case State_Finished:
                        {
                            result = Result_Success;
                            break;
                        }

                        default:
                        {
                            // Error
                            nextState = State_Error;
                            break;
                        }
                    }
                    break;
                }

                default:
                {
                    // Error, invalid state
                    nextState = State_Error;
                    break;
                }
            }

            // Update state
            m_state = nextState;
        }
    }

    if ((result == Result_Success) ||
        (result == Result_Error))
    {
        m_parsingBuffer->eraseToCurrentPosition();
    }

    return result;
}

/**
 * Execute state: Waiting for start of token
 *
 * \retval State_WaitingForStartOfToken Wait for more data
 * \retval State_ReadingTokenType       Start of token found
 * \retval State_Finished               Whitespace found
 * \retval State_Error                  Error, unexpected character
 *
 * Format:
 * \code{.unparsed}
 * Start of token ::= '<' | S
 * \endcode
 */
TokenTypeParser::State TokenTypeParser::executeStateWaitingForStartOfToken()
{
    State nextState = State_Error;
    bool finishParsing = false;

    while (!finishParsing)
    {
        finishParsing = true;

        // Check if more data is needed
        if (m_parsingBuffer->isMoreDataNeeded())
        {
            // More data is needed
            nextState = State_WaitingForStartOfToken;
        }
        else
        {
            // Check character
            const uint32_t uchar = m_parsingBuffer->currentChar();

            if (uchar == static_cast<uint32_t>('<'))
            {
                // Start of token found, now start reading the token type
                m_parsingBuffer->incrementPosition();
                m_parsingBuffer->eraseToCurrentPosition();
                nextState = State_ReadingTokenType;
            }
            else
            {
                if (option() == Option_Synchronization)
                {
                    // On synchronization option ignore all other characters
                    m_parsingBuffer->incrementPosition();
                    m_parsingBuffer->eraseToCurrentPosition();
                    finishParsing = false;
                }
                else
                {
                    if (XmlValidator::isWhitespace(uchar))
                    {
                        if (option() == Option_IgnoreLeadingWhitespace)
                        {
                            // We are allowed to ignore whitespace characters
                            m_parsingBuffer->incrementPosition();
                            m_parsingBuffer->eraseToCurrentPosition();
                            finishParsing = false;
                        }
                        else
                        {
                            // Parsing finished: Whitespace character found
                            setTokenType(TokenType_Whitespace);
                            nextState = State_Finished;
                        }
                    }
                    else
                    {
                        // Error, invalid character read
                        setTerminationChar(uchar);
                    }
                }
            }
        }
    }

    return nextState;
}

/**
 * Execute state: Reading token type
 *
 * \retval State_ReadingTokenType                   Wait for more data
 * \retval State_ReadingTokenTypeExclamationMark    Found a token that starts with an exclamation
 *                                                  mark
 * \retval State_Finished                           Token type found
 * \retval State_Error                              Error, unexpected character
 *
 * Format of valid item types:
 * \code{.unparsed}
 * Processing instruction ::= '<?'
 * Document type          ::= '<!DOCTYPE'
 * Comment                ::= '<!--'
 * CData                  ::= '<![CDATA['
 * Start of element       ::= '<' NameStartChar NameChar*
 * End of element         ::= '</' NameStartChar NameChar*
 * \endcode
 *
 * \note Current position in the parsing buffer points to the character after '<'.
 */
TokenTypeParser::State TokenTypeParser::executeStateReadingTokenType()
{
    State nextState = State_Error;
    bool finishParsing = false;

    while (!finishParsing)
    {
        finishParsing = true;

        // Check if more data is needed
        if (m_parsingBuffer->isMoreDataNeeded())
        {
            // More data is needed
            nextState = State_ReadingTokenType;
        }
        else
        {
            // Check character
            const uint32_t uchar = m_parsingBuffer->currentChar();

            if (uchar == static_cast<uint32_t>('?'))
            {
                // Token found: Processing instruction
                m_parsingBuffer->incrementPosition();
                m_parsingBuffer->eraseToCurrentPosition();
                setTokenType(TokenType_ProcessingInstruction);
                nextState = State_Finished;
            }
            else if (uchar == static_cast<uint32_t>('!'))
            {
                // Start reading a token type with exclamation mark
                m_parsingBuffer->incrementPosition();
                m_parsingBuffer->eraseToCurrentPosition();
                nextState = State_ReadingTokenTypeExclamationMark;
            }
            else if (XmlValidator::isNameStartChar(uchar))
            {
                // Token found: Start of element
                m_parsingBuffer->eraseToCurrentPosition();
                setTokenType(TokenType_StartOfElement);
                nextState = State_Finished;
            }
            else if (uchar == static_cast<uint32_t>('/'))
            {
                // Token found: End of element
                m_parsingBuffer->incrementPosition();
                m_parsingBuffer->eraseToCurrentPosition();
                setTokenType(TokenType_EndOfElement);
                nextState = State_Finished;
            }
            else
            {
                // Error, invalid character
            }
        }
    }

    return nextState;
}

/**
 * Execute state: Reading token type that starts with an exclamation mark
 *
 * \retval State_ReadingTokenTypeExclamationMark    Wait for more data
 * \retval State_ReadingTokenTypeDocumentType       Found start of document type token type
 * \retval State_ReadingTokenTypeComment            Found start of comment token type
 * \retval State_ReadingTokenTypeCData              Found start of CData token type
 * \retval State_Error                              Error, unexpected character
 *
 * Format of valid item types:
 * \code{.unparsed}
 * Document type ::= '<!DOCTYPE'
 * Comment       ::= '<!--'
 * CData         ::= '<![CDATA['
 * \endcode
 *
 * \note Current position in the parsing buffer points to the character after '<!'.
 */
TokenTypeParser::State TokenTypeParser::executeStateReadingTokenTypeExclamationMark()
{
    State nextState = State_Error;
    bool finishParsing = false;

    while (!finishParsing)
    {
        finishParsing = true;

        // Check if more data is needed
        if (m_parsingBuffer->isMoreDataNeeded())
        {
            // More data is needed
            nextState = State_ReadingTokenTypeExclamationMark;
        }
        else
        {
            // Check character
            const uint32_t uchar = m_parsingBuffer->currentChar();

            if (uchar == static_cast<uint32_t>('D'))
            {
                // Start reading document type token type
                m_parsingBuffer->incrementPosition();
                nextState = State_ReadingTokenTypeDocumentType;
            }
            else if (uchar == static_cast<uint32_t>('-'))
            {
                // Start reading comment token type
                m_parsingBuffer->incrementPosition();
                nextState = State_ReadingTokenTypeComment;
            }
            else if (uchar == static_cast<uint32_t>('['))
            {
                // Start reading CData token type
                m_parsingBuffer->incrementPosition();
                nextState = State_ReadingTokenTypeCData;
            }
            else
            {
                // Error, invalid character
            }
        }
    }

    return nextState;
}

/**
 * Execute state: Reading document type token type
 *
 * \retval State_ReadingTokenTypeDocumentType   Wait for more data
 * \retval State_Finished                       Token type found
 * \retval State_Error                          Error, unexpected character
 *
 * Format of valid item types:
 * \code{.unparsed}
 * Document type ::= '<!DOCTYPE'
 * \endcode
 */
TokenTypeParser::State TokenTypeParser::executeStateReadingTokenTypeDocumentType()
{
    static const char s_doctype[] = "DOCTYPE";
    State nextState = State_Error;
    bool finishParsing = false;

    while (!finishParsing)
    {
        finishParsing = true;

        // Check if more data is needed
        if (m_parsingBuffer->isMoreDataNeeded())
        {
            // More data is needed
            nextState = State_ReadingTokenTypeDocumentType;
        }
        else
        {
            // Check character
            const size_t position = m_parsingBuffer->currentPosition();
            const uint32_t uchar = m_parsingBuffer->currentChar();

            if (position < 7U)
            {
                const uint32_t doctypeChar = static_cast<uint32_t>(s_doctype[position]);

                if (doctypeChar == uchar)
                {
                    // Correct character found
                    m_parsingBuffer->incrementPosition();

                    if (position == 6U)
                    {
                        // Token found: Document type
                        m_parsingBuffer->eraseToCurrentPosition();
                        setTokenType(TokenType_DocumentType);
                        nextState = State_Finished;
                    }
                    else
                    {
                        // Check next character
                        finishParsing = false;
                    }
                }
                else
                {
                    // Error, invalid character
                }
            }
            else
            {
                // Error, invalid character
            }
        }
    }

    return nextState;
}

/**
 * Execute state: Reading comment token type
 *
 * \retval State_ReadingTokenTypeComment    Wait for more data
 * \retval State_Finished                   Token type found
 * \retval State_Error                      Error, unexpected character
 *
 * Format of valid item types:
 * \code{.unparsed}
 * Comment ::= '<!--'
 * \endcode
 *
 * \note Current position in the parsing buffer points to the character after '<!-'.
 */
TokenTypeParser::State TokenTypeParser::executeStateReadingTokenTypeComment()
{
    State nextState = State_Error;
    bool finishParsing = false;

    while (!finishParsing)
    {
        finishParsing = true;

        // Check if more data is needed
        if (m_parsingBuffer->isMoreDataNeeded())
        {
            // More data is needed
            nextState = State_ReadingTokenTypeComment;
        }
        else
        {
            // Check character
            const uint32_t uchar = m_parsingBuffer->currentChar();

            if (uchar == static_cast<uint32_t>('-'))
            {
                // Token found: Comment
                m_parsingBuffer->incrementPosition();
                m_parsingBuffer->eraseToCurrentPosition();
                setTokenType(TokenType_Comment);
                nextState = State_Finished;
            }
            else
            {
                // Error, invalid character
            }
        }
    }

    return nextState;
}

/**
 * Execute state: Reading CDATA token type
 *
 * \retval State_ReadingTokenTypeCData  Wait for more data
 * \retval State_Finished               Token type found
 * \retval State_Error                  Error, unexpected character
 *
 * Format of valid item types:
 * \code{.unparsed}
 * CData ::= '<![CDATA['
 * \endcode
 */
TokenTypeParser::State TokenTypeParser::executeStateReadingTokenTypeCData()
{
    static const char s_cdata[] = "[CDATA[";
    State nextState = State_Error;
    bool finishParsing = false;

    while (!finishParsing)
    {
        finishParsing = true;

        // Check if more data is needed
        if (m_parsingBuffer->isMoreDataNeeded())
        {
            // More data is needed
            nextState = State_ReadingTokenTypeCData;
        }
        else
        {
            // Check character
            const size_t position = m_parsingBuffer->currentPosition();
            const uint32_t uchar = m_parsingBuffer->currentChar();

            if (position < 7U)
            {
                const uint32_t cdataChar = static_cast<uint32_t>(s_cdata[position]);

                if (cdataChar == uchar)
                {
                    // Correct character found
                    m_parsingBuffer->incrementPosition();

                    if (position == 6U)
                    {
                        // Token found: CDATA
                        m_parsingBuffer->eraseToCurrentPosition();
                        setTokenType(TokenType_CData);
                        nextState = State_Finished;
                    }
                    else
                    {
                        // Check next character
                        finishParsing = false;
                    }
                }
                else
                {
                    // Error, invalid character
                }
            }
            else
            {
                // Error, invalid character
            }
        }
    }

    return nextState;
}
