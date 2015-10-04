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
 *
 * \param parsingBuffer Pointer to a parsing buffer
 */
TextNodeParser::TextNodeParser(ParsingBuffer *parsingBuffer)
    : AbstractTokenParser(parsingBuffer, Option_None, ParserType_TextNode),
      m_state(State_Idle),
      m_referenceParser(NULL),
      m_text()
{
}

/**
 * Destructor
 */
TextNodeParser::~TextNodeParser()
{
    if (m_referenceParser != NULL)
    {
        delete m_referenceParser;
        m_referenceParser = NULL;
    }
}

/**
 * Check if parser is valid
 *
 * \retval true     Valid
 * \retval false    Invalid
 */
bool TextNodeParser::isValid() const
{
    bool valid = AbstractTokenParser::isValid();

    if (valid)
    {
        switch (m_option)
        {
            case Option_None:
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
                    // Erase all characters to the current position and start reading text
                    m_parsingBuffer->eraseToCurrentPosition();
                    m_text.clear();
                    nextState = State_ReadingText;
                    finishParsing = false;
                    break;
                }

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
 * Get text string
 *
 * \return Text string
 */
EmbeddedStAX::Common::UnicodeString TextNodeParser::text() const
{
    return m_text;
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
        if (m_parsingBuffer->isMoreDataNeeded())
        {
            // More data is needed
            nextState = State_ReadingText;
        }
        else
        {
            // Check character
            const uint32_t uchar = m_parsingBuffer->currentChar();

            if (uchar == static_cast<uint32_t>('<'))
            {
                // Add text
                const size_t size = m_parsingBuffer->currentPosition();
                m_text.append(m_parsingBuffer->substring(0U, size));

                // End of text node found
                m_parsingBuffer->eraseToCurrentPosition();
                nextState = State_Finished;
            }
            else if (uchar == static_cast<uint32_t>('&'))
            {
                // Add text
                const size_t size = m_parsingBuffer->currentPosition();
                m_text.append(m_parsingBuffer->substring(0U, size));

                // Possible start of Reference found, parse it
                m_parsingBuffer->eraseToCurrentPosition();

                if (m_referenceParser != NULL)
                {
                    delete m_referenceParser;
                    m_referenceParser = NULL;
                }

                m_referenceParser = new ReferenceParser(m_parsingBuffer);
                nextState = State_ReadingReference;
            }
            else if (uchar == static_cast<uint32_t>('>'))
            {
                // Check if this character is part of the "]]>" sequence
                const size_t position = m_parsingBuffer->currentPosition();

                if (position < 2U)
                {
                    // Valid text character, continue
                    m_parsingBuffer->incrementPosition();
                    finishParsing = false;
                }
                else
                {
                    const Common::UnicodeString sequence = m_parsingBuffer->substring(position - 2U,
                                                                                      position);

                    if (Common::compareUnicodeString(0U, sequence, std::string("]]>")))
                    {
                        // Error, Invalid sequence
                    }
                    else
                    {
                        // Valid sequence, continue
                        m_parsingBuffer->incrementPosition();
                        finishParsing = false;
                    }
                }
            }
            else
            {
                // Valid character, continue, continue
                m_parsingBuffer->incrementPosition();
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
    const Result result = m_referenceParser->parse();

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
            switch (m_referenceParser->tokenFound())
            {
                case TokenType_EntityReference:
                {
                    // Check the entity reference name
                    const Common::UnicodeString name = m_referenceParser->value();

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
                    m_text.append(m_referenceParser->value());
                    nextState = State_ReadingText;
                    break;
                }

                default:
                {
                    // Error, unexpected token type
                    break;
                }
            }

            // Delete reference parser
            delete m_referenceParser;
            m_referenceParser = NULL;
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
