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

#include <EmbeddedStAX/XmlReader/TokenParsers/EndOfElementParser.h>
#include <EmbeddedStAX/XmlValidator/Common.h>

using namespace EmbeddedStAX::XmlReader;

/**
 * Constructor
 *
 * \param parsingBuffer Pointer to a parsing buffer
 * \param option        Parsing option
 */
EndOfElementParser::EndOfElementParser(ParsingBuffer *parsingBuffer)
    : AbstractTokenParser(parsingBuffer, Option_None, ParserType_Reference),
      m_state(State_Idle),
      m_nameParser(NULL),
      m_elementName()
{
}

/**
 * Destructor
 */
EndOfElementParser::~EndOfElementParser()
{
    if (m_nameParser != NULL)
    {
        delete m_nameParser;
        m_nameParser = NULL;
    }
}

/**
 * Check if parser is valid
 *
 * \retval true     Valid
 * \retval false    Invalid
 */
bool EndOfElementParser::isValid() const
{
    bool valid = AbstractTokenParser::isValid();

    if (valid)
    {
        if (m_option != Option_None)
        {
            valid = false;
        }
    }

    return valid;
}

/**
 * Parse
 *
 * \retval Result_Success       Success
 * \retval Result_NeedMoreData  More data is needed
 * \retval Result_Error         Error
 */
AbstractTokenParser::Result EndOfElementParser::parse()
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
                case State_Idle:
                {
                    // Initialize name parser
                    if (m_nameParser != NULL)
                    {
                        delete m_nameParser;
                        m_nameParser = NULL;
                    }

                    m_nameParser = new NameParser(m_parsingBuffer, Option_None);

                    // Execute another cycle
                    nextState = State_ReadingElementName;
                    finishParsing = false;
                    break;
                }

                case State_ReadingElementName:
                {
                    // Reading element name
                    nextState = executeStateReadingElementName();

                    // Check transitions
                    switch (nextState)
                    {
                        case State_ReadingElementName:
                        {
                            result = Result_NeedMoreData;
                            break;
                        }

                        case State_ReadingEndOfElement:
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

                case State_ReadingEndOfElement:
                {
                    // Reading end of element
                    nextState = executeStateReadingEndOfElement();

                    // Check transitions
                    switch (nextState)
                    {
                        case State_ReadingEndOfElement:
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
 * Get element name
 *
 * \return Element name
 */
EmbeddedStAX::Common::UnicodeString EndOfElementParser::name() const
{
    return m_elementName;
}

/**
 * Execute state: Reading element name
 *
 * \retval State_ReadingElementName     Wait for more data
 * \retval State_ReadingEndOfElement    End of element name found
 * \retval State_Finished               End of element found
 * \retval State_Error                  Error, unexpected character
 *
 * Format:
 * \code{.unparsed}
 * ETag ::= '</' Name S? '>'
 * \endcode
 */
EndOfElementParser::State EndOfElementParser::executeStateReadingElementName()
{
    State nextState = State_Error;

    // Parse
    const Result result = m_nameParser->parse();

    switch (result)
    {
        case Result_NeedMoreData:
        {
            // More data is needed
            nextState = State_ReadingElementName;
            break;
        }

        case Result_Success:
        {
            // Check for end of entity reference
            const uint32_t uchar = m_parsingBuffer->currentChar();

            if (uchar == static_cast<uint32_t>('>'))
            {
                // End of element found
                m_elementName = m_nameParser->value();

                delete m_nameParser;
                m_nameParser = NULL;

                m_parsingBuffer->incrementPosition();
                m_parsingBuffer->eraseToCurrentPosition();
                m_tokenFound = TokenType_EndOfElement;
                nextState = State_Finished;
            }
            else if (XmlValidator::isWhitespace(uchar))
            {
                // End of element name, try to read end of element
                m_elementName = m_nameParser->value();

                delete m_nameParser;
                m_nameParser = NULL;

                m_parsingBuffer->incrementPosition();
                m_parsingBuffer->eraseToCurrentPosition();
                nextState = State_ReadingEndOfElement;
            }
            else
            {
                // Error, invalid character
                m_terminationChar = uchar;
            }
            break;
        }

        default:
        {
            // Error
            break;
        }
    }

    return nextState;
}

/**
 * Execute state: Reading end of element
 *
 * \retval State_ReadingEndOfElement    Wait for more data
 * \retval State_ReadingFinished        End of element found
 * \retval State_Error                  Error, unexpected character
 */
EndOfElementParser::State EndOfElementParser::executeStateReadingEndOfElement()
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
            nextState = State_ReadingEndOfElement;
        }
        else
        {
            // Check character
            const uint32_t uchar = m_parsingBuffer->currentChar();

            if (uchar == static_cast<uint32_t>('>'))
            {
                // End of element found
                m_parsingBuffer->incrementPosition();
                m_parsingBuffer->eraseToCurrentPosition();
                m_tokenFound = TokenType_EndOfElement;
                nextState = State_Finished;
            }
            else if (XmlValidator::isWhitespace(uchar))
            {
                // Ignore trailing whitespace
                m_parsingBuffer->incrementPosition();
                m_parsingBuffer->eraseToCurrentPosition();
                finishParsing = false;
            }
            else
            {
                // Error, invalid character read
                m_terminationChar = uchar;
            }
        }
    }

    return nextState;
}
