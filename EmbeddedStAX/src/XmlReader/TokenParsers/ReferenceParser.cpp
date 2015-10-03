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

#include <EmbeddedStAX/XmlReader/TokenParsers/ReferenceParser.h>
#include <EmbeddedStAX/Common/Common.h>
#include <EmbeddedStAX/XmlValidator/Common.h>
#include <EmbeddedStAX/XmlValidator/Name.h>

using namespace EmbeddedStAX::XmlReader;

/**
 * Constructor
 *
 * \param parsingBuffer Pointer to a parsing buffer
 * \param option        Parsing option
 */
ReferenceParser::ReferenceParser(ParsingBuffer *parsingBuffer)
    : AbstractTokenParser(parsingBuffer, Option_None, ParserType_Reference),
      m_state(State_ReadingStartOfReference),
      m_nameParser(NULL),
      m_value(),
      m_charRefValue(0U)
{
}

/**
 * Destructor
 */
ReferenceParser::~ReferenceParser()
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
bool ReferenceParser::isValid() const
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
AbstractTokenParser::Result ReferenceParser::parse()
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
                case State_ReadingStartOfReference:
                {
                    // Reading start of reference
                    nextState = executeStateReadingStartOfReference();

                    // Check transitions
                    switch (nextState)
                    {
                        case State_ReadingStartOfReference:
                        {
                            result = Result_NeedMoreData;
                            break;
                        }

                        case State_ReadingReferenceType:
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

                case State_ReadingReferenceType:
                {
                    // Reading reference type
                    nextState = executeStateReadingReferenceType();

                    // Check transitions
                    switch (nextState)
                    {
                        case State_ReadingReferenceType:
                        {
                            result = Result_NeedMoreData;
                            break;
                        }

                        case State_ReadingEntityReferenceName:
                        case State_ReadingCharacterReferenceType:
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

                case State_ReadingEntityReferenceName:
                {
                    // Reading entity reference name
                    nextState = executeStateReadingEntityReferenceName();

                    // Check transitions
                    switch (nextState)
                    {
                        case State_ReadingEntityReferenceName:
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

                case State_ReadingCharacterReferenceType:
                {
                    // Reading character reference type
                    nextState = executeStateReadingCharacterReferenceType();

                    // Check transitions
                    switch (nextState)
                    {
                        case State_ReadingCharacterReferenceType:
                        {
                            result = Result_NeedMoreData;
                            break;
                        }

                        case State_ReadingCharacterReferenceDecimal:
                        case State_ReadingCharacterReferenceHexadecimal:
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

                case State_ReadingCharacterReferenceDecimal:
                {
                    // Reading character reference - in decimal format
                    nextState = executeStateReadingCharacterReferenceDecimal();

                    // Check transitions
                    switch (nextState)
                    {
                        case State_ReadingCharacterReferenceDecimal:
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

                case State_ReadingCharacterReferenceHexadecimal:
                {
                    // Reading character reference - in hexadecimal format
                    nextState = executeStateReadingCharacterReferenceHexadecimal();

                    // Check transitions
                    switch (nextState)
                    {
                        case State_ReadingCharacterReferenceHexadecimal:
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
EmbeddedStAX::Common::UnicodeString ReferenceParser::value() const
{
    return m_value;
}

/**
 * Execute state: Reading start of reference
 *
 * \retval State_ReadingStartOfReference    Wait for more data
 * \retval State_ReadingReferenceType       Start of reference found
 * \retval State_Error                      Error, unexpected character
 *
 * \note A Reference has to start with a '&' character.
 */
ReferenceParser::State ReferenceParser::executeStateReadingStartOfReference()
{
    State nextState = State_Error;

    // Check if more data is needed
    if (m_parsingBuffer->isMoreDataNeeded())
    {
        // More data is needed
        nextState = State_ReadingStartOfReference;
    }
    else
    {
        // Check character
        const uint32_t uchar = m_parsingBuffer->currentChar();

        if (uchar == static_cast<uint32_t>('&'))
        {
            // Start of reference found, now start reading the referece type
            m_parsingBuffer->incrementPosition();
            m_parsingBuffer->eraseToCurrentPosition();
            nextState = State_ReadingReferenceType;
        }
        else
        {
            // Error, invalid character read
            m_terminationChar = uchar;
        }
    }

    return nextState;
}

/**
 * Execute state: Reading reference type
 *
 * \retval State_ReadingReferenceType           Wait for more data
 * \retval State_ReadingEntityReferenceName     Entity reference found
 * \retval State_ReadingCharacterReferenceType  Character reference found
 * \retval State_Error                          Error, unexpected character
 *
 * \note A Entity Reference starts with a NameStartChar and a Character Reference starts with a '#'
 *       character.
 */
ReferenceParser::State ReferenceParser::executeStateReadingReferenceType()
{
    State nextState = State_Error;

    // Check if more data is needed
    if (m_parsingBuffer->isMoreDataNeeded())
    {
        // More data is needed
        nextState = State_ReadingStartOfReference;
    }
    else
    {
        // Check character
        const uint32_t uchar = m_parsingBuffer->currentChar();

        if (uchar == static_cast<uint32_t>('#'))
        {
            // Character reference found, now start reading the character referece type
            m_parsingBuffer->incrementPosition();
            m_parsingBuffer->eraseToCurrentPosition();
            nextState = State_ReadingCharacterReferenceType;
        }
        else if (XmlValidator::isNameStartChar(uchar))
        {
            // Entity reference found, now start reading the entity reference name
            if (m_nameParser != NULL)
            {
                delete m_nameParser;
            }

            m_nameParser = new NameParser(m_parsingBuffer);
            nextState = State_ReadingEntityReferenceName;
        }
        else
        {
            // Error, invalid character read
            m_terminationChar = uchar;
        }
    }

    return nextState;
}

/**
 * Execute state: Reading entity reference name
 *
 * \retval State_ReadingEntityReferenceName Wait for more data
 * \retval State_Finished                   Entity reference found
 * \retval State_Error                      Error, unexpected character
 *
 * Format:
 * \code{.unparsed}
 * EntityRef ::= '&' Name ';'
 * \endcode
 */
ReferenceParser::State ReferenceParser::executeStateReadingEntityReferenceName()
{
    State nextState = State_Error;

    // Parse
    const Result result = m_nameParser->parse();

    switch (result)
    {
        case Result_NeedMoreData:
        {
            // More data is needed
            nextState = State_ReadingEntityReferenceName;
            break;
        }

        case Result_Success:
        {
            // Check for end of entity reference
            const uint32_t uchar = m_parsingBuffer->currentChar();

            if (uchar == static_cast<uint32_t>(';'))
            {
                // End of entity reference found
                m_value = m_nameParser->value();

                delete m_nameParser;
                m_nameParser = NULL;

                m_parsingBuffer->incrementPosition();
                m_parsingBuffer->eraseToCurrentPosition();
                m_tokenFound = TokenType_EntityReference;
                nextState = State_Finished;
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
 * Execute state: Reading character reference type
 *
 * \retval State_ReadingCharacterReferenceType          Wait for more data
 * \retval State_ReadingCharacterReferenceDecimal       Decimal character reference found
 * \retval State_ReadingCharacterReferenceHexadecimal   Hexadecimal character reference found
 * \retval State_Error                                  Error, unexpected character
 *
 * Format:
 * \code{.unparsed}
 * CharRef ::= '&#' [0-9]+ ';'
            |  '&#x' [0-9a-fA-F]+ ';'
 * \endcode
 */
ReferenceParser::State ReferenceParser::executeStateReadingCharacterReferenceType()
{
    State nextState = State_Error;

    // Check if more data is needed
    if (m_parsingBuffer->isMoreDataNeeded())
    {
        // More data is needed
        nextState = State_ReadingStartOfReference;
    }
    else
    {
        // Check character
        const uint32_t uchar = m_parsingBuffer->currentChar();

        if (uchar == static_cast<uint32_t>('x'))
        {
            // Hexadecimal character reference found, now start reading it
            m_parsingBuffer->incrementPosition();
            m_parsingBuffer->eraseToCurrentPosition();
            m_charRefValue = 0U;
            nextState = State_ReadingCharacterReferenceHexadecimal;
        }
        else if ((static_cast<uint32_t>('0') <= uchar) && (uchar <= static_cast<uint32_t>('9')))
        {
            // Decimal character reference found, now start reading it
            m_charRefValue = 0U;
            nextState = State_ReadingCharacterReferenceDecimal;
        }
        else
        {
            // Error, invalid character read
            m_terminationChar = uchar;
        }
    }

    return nextState;
}

/**
 * Execute state: Reading character reference - in decimal format
 *
 * \retval State_ReadingCharacterReferenceDecimal   Wait for more data
 * \retval State_Finished                           Character reference found
 * \retval State_Error                              Error, unexpected character
 *
 * Format:
 * \code{.unparsed}
 * CharRef ::= '&#' [0-9]+ ';'
 * \endcode
 */
ReferenceParser::State ReferenceParser::executeStateReadingCharacterReferenceDecimal()
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
            nextState = State_ReadingCharacterReferenceDecimal;
        }
        else
        {
            // Check character
            const uint32_t uchar = m_parsingBuffer->currentChar();
            uint32_t digitValue;

            if (uchar == static_cast<uint32_t>(';'))
            {
                // End of character reference (in decimal format) found
                m_value.clear();
                m_value.push_back(uchar);

                m_parsingBuffer->incrementPosition();
                m_parsingBuffer->eraseToCurrentPosition();
                m_tokenFound = TokenType_CharacterReference;
                nextState = State_Finished;
            }
            else if (Common::parseDigit(uchar, 10U, &digitValue))
            {
                // Digit found
                m_charRefValue = (m_charRefValue * 10U) + uchar;

                if (Common::isUnicodeChar(m_charRefValue))
                {
                    // Check next digit
                    m_parsingBuffer->incrementPosition();
                    finishParsing = false;
                }
                else
                {
                    // Error, invalid character value
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
 * Execute state: Reading character reference - in hexadecimal format
 *
 * \retval State_ReadingCharacterReferenceHexadecimal   Wait for more data
 * \retval State_Finished                               Character reference found
 * \retval State_Error                                  Error, unexpected character
 *
 * Format:
 * \code{.unparsed}
 * CharRef ::= '&#x' [0-9a-fA-F]+ ';'
 * \endcode
 */
ReferenceParser::State ReferenceParser::executeStateReadingCharacterReferenceHexadecimal()
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
            nextState = State_ReadingCharacterReferenceHexadecimal;
        }
        else
        {
            // Check character
            const uint32_t uchar = m_parsingBuffer->currentChar();
            uint32_t digitValue;

            if (uchar == static_cast<uint32_t>(';'))
            {
                // End of character reference (in hexadecimal format) found
                m_value.clear();
                m_value.push_back(uchar);

                m_parsingBuffer->incrementPosition();
                m_parsingBuffer->eraseToCurrentPosition();
                m_tokenFound = TokenType_CharacterReference;
                nextState = State_Finished;
            }
            else if (Common::parseDigit(uchar, 16U, &digitValue))
            {
                // Digit found
                m_charRefValue = (m_charRefValue * 16U) + uchar;

                if (Common::isUnicodeChar(m_charRefValue))
                {
                    // Check next digit
                    m_parsingBuffer->incrementPosition();
                    finishParsing = false;
                }
                else
                {
                    // Error, invalid character value
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
