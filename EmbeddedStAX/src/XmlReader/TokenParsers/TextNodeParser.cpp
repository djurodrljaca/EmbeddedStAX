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

#include <EmbeddedStAX/XmlReader/TokenParsers/TextNodeParser.h>
#include <EmbeddedStAX/XmlValidator/Common.h>

using namespace EmbeddedStAX::XmlReader;

/**
 * Constructor
 */
TextNodeParser::TextNodeParser()
    : AbstractTokenParser(ParserType_TextNode),
      m_state(State_ReadingText),
      m_referenceParser(),
      m_text()
{
}

/**
 * Destructor
 */
TextNodeParser::~TextNodeParser()
{
}

/**
 * Get text string
 *
 * \return Text string
 */
EmbeddedStAX::Common::UnicodeString TextNodeParser::text() const
{
    return m_text;
}

/**
 * Parse
 *
 * \retval Result_Success       Success
 * \retval Result_NeedMoreData  More data is needed
 * \retval Result_Error         Error
 *
 * Format:
 * \code{.unparsed}
 * content ::= CharData? ((element | Reference | CDSect | PI | Comment) CharData?)*
 * \endcode
 */
AbstractTokenParser::Result TextNodeParser::parse()
{
    Result result = Result_Error;

    if (isInitialized())
    {
        bool finishParsing = false;

        while (!finishParsing)
        {
            finishParsing = true;
            State nextState = State_Error;

            switch (m_state)
            {
                case State_ReadingText:
                {
                    // Reading text
                    nextState = executeStateReadingText();

                    // Check transitions
                    switch (nextState)
                    {
                        case State_ReadingText:
                        {
                            result = Result_NeedMoreData;
                            break;
                        }

                        case State_ReadingReference:
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

                case State_ReadingReference:
                {
                    // Reading reference
                    nextState = executeStateReadingReference();

                    // Check transitions
                    switch (nextState)
                    {
                        case State_ReadingReference:
                        {
                            result = Result_NeedMoreData;
                            break;
                        }

                        case State_ReadingText:
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

                case State_Finished:
                {
                    result = Result_Success;
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
        parsingBuffer()->eraseToCurrentPosition();
    }

    return result;
}

/**
 * Initialize parser's additional data
 *
 * \retval true     Success
 * \retval false    Error
 */
bool TextNodeParser::initializeAdditionalData()
{
    m_state = State_ReadingText;
    m_text.clear();
    parsingBuffer()->eraseToCurrentPosition();
    m_referenceParser.deinitialize();
    return true;
}

/**
 * Deinitialize parser's additional data
 */
void TextNodeParser::deinitializeAdditionalData()
{
    m_state = State_ReadingText;
    m_text.clear();
    m_referenceParser.deinitialize();
}

/**
 * Execute state: Reading text
 *
 * \retval State_ReadingText        Wait for more data
 * \retval State_ReadingReference   Start of reference found
 * \retval State_Finished           End of text node found
 * \retval State_Error              Error, unexpected character
 *
 * Format:
 * \code{.unparsed}
 * Text node ::= CharData? (Reference CharData?)*
 * CharData  ::= [^<&]* - ([^<&]* ']]>' [^<&]*)
 * \endcode
 */
TextNodeParser::State TextNodeParser::executeStateReadingText()
{
    State nextState = State_Error;
    bool finishParsing = false;

    while (!finishParsing)
    {
        finishParsing = true;

        // Check if more data is needed
        if (parsingBuffer()->isMoreDataNeeded())
        {
            // More data is needed
            nextState = State_ReadingText;
        }
        else
        {
            // Check character
            const uint32_t uchar = parsingBuffer()->currentChar();

            if (uchar == static_cast<uint32_t>('<'))
            {
                // Add text
                const size_t size = parsingBuffer()->currentPosition();
                m_text.append(parsingBuffer()->substring(0U, size));

                // TODO: validate?

                // End of text node found
                parsingBuffer()->eraseToCurrentPosition();
                nextState = State_Finished;
            }
            else if (uchar == static_cast<uint32_t>('&'))
            {
                // Add text
                const size_t size = parsingBuffer()->currentPosition();
                m_text.append(parsingBuffer()->substring(0U, size));

                // Possible start of Reference found, parse it
                parsingBuffer()->eraseToCurrentPosition();
                m_referenceParser.initialize(parsingBuffer());
                nextState = State_ReadingReference;
            }
            else if (uchar == static_cast<uint32_t>('>'))
            {
                // Check if this character is part of the "]]>" sequence
                const size_t position = parsingBuffer()->currentPosition();

                if (position < 2U)
                {
                    // Valid text character, continue
                    parsingBuffer()->incrementPosition();
                    finishParsing = false;
                }
                else
                {
                    const Common::UnicodeString sequence = parsingBuffer()->substring(position - 2U,
                                                                                      position);

                    if (Common::compareUnicodeString(0U, sequence, std::string("]]>")))
                    {
                        // Error, Invalid sequence
                    }
                    else
                    {
                        // Valid sequence, continue
                        parsingBuffer()->incrementPosition();
                        finishParsing = false;
                    }
                }
            }
            else
            {
                // Valid character, continue, continue
                parsingBuffer()->incrementPosition();
                finishParsing = false;
            }
        }
    }

    return nextState;
}

/**
 * Execute state: Reading reference
 *
 * \retval State_ReadingReference   Wait for more data
 * \retval State_ReadingText        End of a valid reference found
 * \retval State_Error              Error, invalid reference
 */
TextNodeParser::State TextNodeParser::executeStateReadingReference()
{
    State nextState = State_Error;

    // Parse
    const Result result = m_referenceParser.parse();

    switch (result)
    {
        case Result_NeedMoreData:
        {
            // More data is needed
            nextState = State_ReadingReference;
            break;
        }

        case Result_Success:
        {
            // End of reference found
            switch (m_referenceParser.tokenType())
            {
                case TokenType_EntityReference:
                {
                    // Check the entity reference name
                    const Common::UnicodeString name = m_referenceParser.value();

                    if (Common::compareUnicodeString(0U, name, std::string("amp")))
                    {
                        // Reference to '&' character found, add it to the value
                        m_text.push_back(static_cast<uint32_t>('&'));
                    }
                    else if (Common::compareUnicodeString(0U, name, std::string("lt")))
                    {
                        // Reference to '<' character found, add it to the value
                        m_text.push_back(static_cast<uint32_t>('<'));
                    }
                    else if (Common::compareUnicodeString(0U, name, std::string("gt")))
                    {
                        // Reference to '>' character found, add it to the value
                        m_text.push_back(static_cast<uint32_t>('>'));
                    }
                    else if (Common::compareUnicodeString(0U, name, std::string("apos")))
                    {
                        // Reference to "'" character found, add it to the value
                        m_text.push_back(static_cast<uint32_t>('\''));
                    }
                    else if (Common::compareUnicodeString(0U, name, std::string("quot")))
                    {
                        // Reference to '"' character found, add it to the value
                        m_text.push_back(static_cast<uint32_t>('"'));
                    }
                    else
                    {
                        // Unknown entity reference, add the full entity reference to the value
                        m_text.push_back(static_cast<uint32_t>('&'));
                        m_text.append(name);
                        m_text.push_back(static_cast<uint32_t>(';'));
                    }

                    nextState = State_ReadingText;
                    break;
                }

                case TokenType_CharacterReference:
                {
                    // Add the character from the character reference to the value
                    m_text.append(m_referenceParser.value());
                    nextState = State_ReadingText;
                    break;
                }

                default:
                {
                    // Error, unexpected token type
                    break;
                }
            }

            m_referenceParser.deinitialize();
            break;
        }

        default:
        {
            // Error
            m_referenceParser.deinitialize();
            break;
        }
    }

    return nextState;
}
