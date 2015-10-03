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

#include <EmbeddedStAX/XmlReader/TokenParsers/AttributeValueParser.h>
#include <EmbeddedStAX/XmlReader/TokenParsers/ReferenceParser.h>
#include <EmbeddedStAX/XmlValidator/Common.h>

using namespace EmbeddedStAX::XmlReader;

/**
 * Constructor
 *
 * \param parsingBuffer Pointer to a parsing buffer
 * \param option        Parsing option
 */
AttributeValueParser::AttributeValueParser(ParsingBuffer *parsingBuffer, Option option)
    : AbstractTokenParser(parsingBuffer, option, ParserType_AttributeValue),
      m_state(State_ReadingQuotationMark),
      m_referenceParser(NULL),
      m_value(),
      m_quotationMark(Common::QuotationMark_None)
{
}

/**
 * Destructor
 */
AttributeValueParser::~AttributeValueParser()
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
bool AttributeValueParser::isValid() const
{
    bool valid = AbstractTokenParser::isValid();

    if (valid)
    {
        switch (m_option)
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
bool AttributeValueParser::setOption(const AbstractTokenParser::Option parsingOption)
{
    bool success = false;

    switch (parsingOption)
    {
        case Option_None:
        case Option_IgnoreLeadingWhitespace:
        {
            // Valid option
            m_option = parsingOption;
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
AbstractTokenParser::Result AttributeValueParser::parse()
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
                case State_ReadingQuotationMark:
                {
                    // Reading quotation mark
                    nextState = executeStateReadingQuotationMark();

                    // Check transitions
                    switch (nextState)
                    {
                        case State_ReadingQuotationMark:
                        {
                            result = Result_NeedMoreData;
                            break;
                        }

                        case State_ReadingAttributeValue:
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

                case State_ReadingAttributeValue:
                {
                    // Reading attribute value
                    nextState = executeStateReadingAttributeValue();

                    // Check transitions
                    switch (nextState)
                    {
                        case State_ReadingAttributeValue:
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

                        case State_ReadingAttributeValue:
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
 * Get value string
 *
 * \return Value string
 */
EmbeddedStAX::Common::UnicodeString AttributeValueParser::value() const
{
    return m_value;
}

/**
 * Execute state: Reading quotation mark
 *
 * \retval State_ReadingQuotationMark   Wait for more data
 * \retval State_ReadingAttributeValue  Quotation mark found
 * \retval State_Error                  Error, unexpected character
 *
 * Format:
 * \code{.unparsed}
 * Quotation mark ::= ('"' | "'")
 * \endcode
 */
AttributeValueParser::State AttributeValueParser::executeStateReadingQuotationMark()
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
            nextState = State_ReadingAttributeValue;
        }
        else
        {
            // Check character
            const uint32_t uchar = m_parsingBuffer->currentChar();

            if (uchar == static_cast<uint32_t>('"'))
            {
                // Quote found, now start reading the attribute value
                m_quotationMark = Common::QuotationMark_Quote;
                m_parsingBuffer->incrementPosition();
                m_parsingBuffer->eraseToCurrentPosition();
                nextState = State_ReadingAttributeValue;
            }
            else if (uchar == static_cast<uint32_t>('\''))
            {
                // Apostrophe found, now start reading the attribute value
                m_quotationMark = Common::QuotationMark_Apostrophe;
                m_parsingBuffer->incrementPosition();
                m_parsingBuffer->eraseToCurrentPosition();
                nextState = State_ReadingAttributeValue;
            }
            else if (XmlValidator::isWhitespace(uchar))
            {
                if (m_option == Option_IgnoreLeadingWhitespace)
                {
                    // Ignore leading whitespace
                    finishParsing = false;
                }
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

/**
 * Execute state: Reading attribute value
 *
 * \retval State_ReadingAttributeValue  Wait for more data
 * \retval State_ReadingReference       Start of reference found
 * \retval State_Finished               End of attribute value found
 * \retval State_Error                  Error, unexpected character
 *
 * Format:
 * \code{.unparsed}
 * AttValue ::= '"' ([^<&"] | Reference)* '"'
 *           |  "'" ([^<&'] | Reference)* "'"
 * \endcode
 */
AttributeValueParser::State AttributeValueParser::executeStateReadingAttributeValue()
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
            nextState = State_ReadingAttributeValue;
        }
        else
        {
            // Check character
            const uint32_t uchar = m_parsingBuffer->currentChar();

            if (uchar == static_cast<uint32_t>('<'))
            {
                // Invalid character found
                m_parsingBuffer->eraseToCurrentPosition();
            }
            else if (uchar == static_cast<uint32_t>('&'))
            {
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
            else if (uchar == static_cast<uint32_t>('"'))
            {
                m_parsingBuffer->incrementPosition();

                if (m_quotationMark == Common::QuotationMark_Quote)
                {
                    // End of attribute value found
                    m_parsingBuffer->eraseToCurrentPosition();
                    nextState = State_Finished;
                }
                else
                {
                    // Add character to value
                    m_value.push_back(uchar);
                    finishParsing = false;
                }
            }
            else if (uchar == static_cast<uint32_t>('\''))
            {
                m_parsingBuffer->incrementPosition();

                if (m_quotationMark == Common::QuotationMark_Apostrophe)
                {
                    // End of attribute value found
                    m_parsingBuffer->eraseToCurrentPosition();
                    nextState = State_Finished;
                }
                else
                {
                    // Add character to value
                    m_value.push_back(uchar);
                    finishParsing = false;
                }
            }
            else
            {
                // Add character to value
                m_parsingBuffer->incrementPosition();
                m_value.push_back(uchar);
                finishParsing = false;
            }
        }
    }

    return nextState;
}

/**
 * Execute state: Reading reference
 *
 * \retval State_ReadingReference       Wait for more data
 * \retval State_ReadingAttributeValue  End of a valid reference found
 * \retval State_Error                  Error, invalid reference
 */
AttributeValueParser::State AttributeValueParser::executeStateReadingReference()
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
            // Check for end of entity reference
            const uint32_t terminationChar = m_referenceParser->terminationChar();

            if (terminationChar == static_cast<uint32_t>(';'))
            {
                // End of reference found
                switch (m_referenceParser->tokenFound())
                {
                    case TokenType_EntityReference:
                    {
                        // Check the entity reference name
                        const Common::UnicodeString name = m_referenceParser->value();

                        if (Common::compareUnicodeString(0U, name, "amp"))
                        {
                            // Reference to '&' character found, add it to the value
                            m_value.push_back(static_cast<uint32_t>('&'));
                        }
                        else if (Common::compareUnicodeString(0U, name, "lt"))
                        {
                            // Reference to '<' character found, add it to the value
                            m_value.push_back(static_cast<uint32_t>('<'));
                        }
                        else if (Common::compareUnicodeString(0U, name, "gt"))
                        {
                            // Reference to '>' character found, add it to the value
                            m_value.push_back(static_cast<uint32_t>('>'));
                        }
                        else if (Common::compareUnicodeString(0U, name, "apos"))
                        {
                            // Reference to "'" character found, add it to the value
                            m_value.push_back(static_cast<uint32_t>('\''));
                        }
                        else if (Common::compareUnicodeString(0U, name, "quot"))
                        {
                            // Reference to '"' character found, add it to the value
                            m_value.push_back(static_cast<uint32_t>('"'));
                        }
                        else
                        {
                            // Unknown entity reference, add the full entity reference to the value
                            m_value.push_back(static_cast<uint32_t>('&'));
                            m_value.append(name);
                            m_value.push_back(static_cast<uint32_t>(';'));
                        }

                        nextState = State_ReadingAttributeValue;
                        break;
                    }

                    case TokenType_CharacterReference:
                    {
                        // Add the character from the character reference to the value
                        m_value.append(m_referenceParser->value());
                        nextState = State_ReadingAttributeValue;
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
