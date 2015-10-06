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

#include <EmbeddedStAX/XmlReader/TokenParsers/NameParser.h>
#include <EmbeddedStAX/XmlValidator/Common.h>
#include <EmbeddedStAX/XmlValidator/Name.h>

using namespace EmbeddedStAX::XmlReader;

/**
 * Constructor
 *
 * \param parsingBuffer Pointer to a parsing buffer
 * \param option        Parsing option
 */
NameParser::NameParser(ParsingBuffer *parsingBuffer, Option option)
    : AbstractTokenParser(parsingBuffer, option, ParserType_Name),
      m_state(State_ReadingNameStartChar),
      m_value()
{
}

/**
 * Destructor
 */
NameParser::~NameParser()
{
}

/**
 * Check if parser is valid
 *
 * \retval true     Valid
 * \retval false    Invalid
 */
bool NameParser::isValid() const
{
    bool valid = AbstractTokenParser::isValid();

    if (valid)
    {
        switch (option())
        {
            case Option_None:
            case Option_IgnoreLeadingWhitespace:
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
bool NameParser::setOption(const AbstractTokenParser::Option parsingOption)
{
    bool success = false;

    switch (parsingOption)
    {
        case Option_None:
        case Option_IgnoreLeadingWhitespace:
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
AbstractTokenParser::Result NameParser::parse()
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
                case State_ReadingNameStartChar:
                {
                    // Reading name start character
                    nextState = executeStateReadingNameStartChar();

                    // Check transitions
                    switch (nextState)
                    {
                        case State_ReadingNameStartChar:
                        {
                            result = Result_NeedMoreData;
                            break;
                        }

                        case State_ReadingNameChars:
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

                case State_ReadingNameChars:
                {
                    // Reading name characters
                    nextState = executeStateReadingNameChars();

                    // Check transitions
                    switch (nextState)
                    {
                        case State_ReadingNameChars:
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
 * Get value string
 *
 * \return Value string
 */
EmbeddedStAX::Common::UnicodeString NameParser::value() const
{
    return m_value;
}

/**
 * Execute state: Reading name start character
 *
 * \retval State_ReadingNameStartChar   Wait for more data
 * \retval State_ReadingNameChars       Name start char found
 * \retval State_Error                  Error, unexpected character
 */
NameParser::State NameParser::executeStateReadingNameStartChar()
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
            nextState = State_ReadingNameStartChar;
        }
        else
        {
            // Check character
            const uint32_t uchar = m_parsingBuffer->currentChar();

            if (XmlValidator::isNameStartChar(uchar))
            {
                // Name start character found, now start reading the token type
                m_parsingBuffer->eraseToCurrentPosition();
                m_parsingBuffer->incrementPosition();
                nextState = State_ReadingNameChars;
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
                        // Error, invalid character read
                        setTerminationChar(uchar);
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

    return nextState;
}

/**
 * Execute state: Reading name characters
 *
 * \retval State_ReadingNameChars   Wait for more data
 * \retval State_Finished           End of name found
 * \retval State_Error              Error
 */
NameParser::State NameParser::executeStateReadingNameChars()
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
            nextState = State_ReadingNameChars;
        }
        else
        {
            // Check character
            const uint32_t uchar = m_parsingBuffer->currentChar();

            if (XmlValidator::isNameChar(uchar))
            {
                // Name character found, check for next one
                m_parsingBuffer->incrementPosition();
                finishParsing = false;
            }
            else
            {
                // End of name found
                const size_t size = m_parsingBuffer->currentPosition();
                m_value = m_parsingBuffer->substring(0U, size);

                m_parsingBuffer->eraseToCurrentPosition();
                nextState = State_Finished;
            }
        }
    }

    return nextState;
}
