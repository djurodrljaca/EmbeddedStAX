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

#include "XmlItemParser.h"
#include "Utf.h"
#include "XmlValidator.h"

using namespace MiniSaxCpp;

/**
 * Constructor
 */
XmlItemParser::XmlItemParser(const size_t dataBufferSize)
    : m_xmlDataBuffer(dataBufferSize)
{
    clear();
}

/**
 * Clear internal state and prepare for the first parsing session
 */
void XmlItemParser::clear()
{
    m_xmlDataBuffer.clear();
    clearParsingBuffer();

    m_value.clear();
    m_itemType = ItemType_None;
    m_terminationCharacter = 0U;
}

/**
 * Write data to the data buffer
 *
 * \param data  Data to write
 *
 * \retval true     Success
 * \retval false    Error, data buffer is full
 */
bool XmlItemParser::writeData(const char data)
{
    return m_xmlDataBuffer.write(data);
}

/**
 * Execute parser: Search for a start of an XML item (or a whitespace character)
 *
 * \param option    Parsing option. Supported options:
 *                  - Synchronization: discard all character until a start of an XML item is found
 *                  - IgnoreWhitespace: ignore leading whitespace before a start of an XML item is
 *                                      found
 *
 * \retval Result_NeedMoreData  More data is needed to complete the parsing session
 * \retval Result_Success       Start of an XML item found
 * \retval Result_Success       Whitespace character found
 * \retval Result_Error         Error occurred (invalid character read)
 */
XmlItemParser::Result XmlItemParser::parseStartOfItem(const XmlItemParser::Option option)
{
    Result result = Result_Error;
    bool finishParsing = false;

    do
    {
        if (m_xmlDataBuffer.empty())
        {
            // Wait for more data
            finishParsing = true;
            result = Result_NeedMoreData;
        }
        else
        {
            // Read and parse data
            const char value = m_xmlDataBuffer.read();

            if (value == '<')
            {
                // Parsing finished: Start of an XML item found
                m_terminationCharacter = (uint32_t)value;
                result = Result_Success;
                finishParsing = true;
            }
            else
            {
                if (option == Option_Synchronization)
                {
                    // On synchronization option ignore all other characters
                }
                else
                {
                    if (XmlValidator::isWhitespace((uint32_t)value))
                    {
                        if (option == Option_IgnoreWhitespace)
                        {
                            // We are allowed to ignore whitespace characters
                        }
                        else
                        {
                            // Parsing finished: Whitespace character found
                            m_terminationCharacter = (uint32_t)value;
                            result = Result_Success;
                            finishParsing = true;
                        }
                    }
                    else
                    {
                        // Error, invalid character read
                        finishParsing = true;
                        break;
                    }
                }
            }
        }
    }
    while (!finishParsing);

    return result;
}

/**
 * Execute parser: Parse XML item's type
 *
 * \retval Result_NeedMoreData  More data is needed to complete the parsing session
 * \retval Result_Success       Item type: Processing Instruction
 * \retval Result_Success       Item type: Document Type
 * \retval Result_Success       Item type: Comment
 * \retval Result_Success       Item type: Element
 * \retval Result_Error         Error occurred (invalid character read)
 *
 * \note If Element item type is found then the parsing buffer MUST NOT be cleared as it will
 *       contain the character of the element's name!
 */
XmlItemParser::Result XmlItemParser::parseItemType()
{
    Result result = Result_Error;
    bool finishParsing = false;

    do
    {
        if (m_xmlDataBuffer.empty())
        {
            // Wait for more data
            finishParsing = true;
            result = Result_NeedMoreData;
        }
        else
        {
            // Read and parse data
            const char value = m_xmlDataBuffer.read();

            if (m_position == 0U)
            {
                // Parse first (unicode) character. Possible item types:
                // - Processing Instruction: "?"
                // - Document Type: "!DOCTYPE"
                // - Comment: "!--"
                // - Element: NameStartChar
                if (value == '?')
                {
                    // Item type: Processing Instruction
                    clearParsingBuffer();
                    m_itemType = ItemType_ProcessingInstruction;
                    result = Result_Success;
                    finishParsing = true;
                }
                else if (value == '!')
                {
                    // Possible types: Document Type and Comment
                    m_parsingBuffer.append(1U, value);
                    m_position = 1U;
                }
                else
                {
                    // Get unicode character
                    m_parsingBuffer.append(1U, value);
                    uint32_t unicodeCharacter = 0U;
                    size_t nextPosition = 0U;
                    Utf::Result utfResult = Utf::unicodeCharacterFromUtf8(m_parsingBuffer,
                                                                          m_position,
                                                                          &nextPosition,
                                                                          &unicodeCharacter);

                    if (utfResult == Utf::Result_Success)
                    {
                        if (XmlValidator::isNameStartChar(unicodeCharacter))
                        {
                            // Item type: Element
                            m_itemType = ItemType_Element;
                            result = Result_Success;
                            finishParsing = true;
                        }
                    }
                    else if (utfResult == Utf::Result_Incomplete)
                    {
                        // Read next character and repeat
                    }
                    else
                    {
                        // Error, invalid character read
                        clearParsingBuffer();
                        m_terminationCharacter = (uint32_t)value;
                        finishParsing = true;
                    }
                }
            }
            else
            {
                // Parse the rest of (unicode) characters. Possible item types:
                // - Document Type: "!DOCTYPE"
                // - Comment: "!--"
                m_parsingBuffer.append(1U, value);

                const size_t size = m_parsingBuffer.size();
                m_position = size;
                bool checkNextType = true;

                // Check for Comment
                if (size <= 3U)
                {
                    if (m_parsingBuffer.compare(0U, size, "!--", size) == 0)
                    {
                        if (size == 3U)
                        {
                            // Item type: Comment
                            clearParsingBuffer();
                            m_itemType = ItemType_Comment;
                            result = Result_Success;
                            finishParsing = true;
                        }
                        else
                        {
                            // More data is needed
                        }

                        checkNextType = false;
                    }
                }

                // Check for Document Type
                if ((size <= 8U) && checkNextType)
                {
                    if (m_parsingBuffer.compare(0U, size, "!DOCTYPE", size) == 0)
                    {
                        if (size == 8U)
                        {
                            // Item type: Document Type
                            clearParsingBuffer();
                            m_itemType = ItemType_DocumentType;
                            result = Result_Success;
                            finishParsing = true;
                        }
                        else
                        {
                            // More data is needed
                        }

                        checkNextType = false;
                    }
                }

                if (checkNextType)
                {
                    // Error, invalid character read
                    clearParsingBuffer();
                    m_terminationCharacter = (uint32_t)value;
                    finishParsing = true;
                }
            }

        }
    }
    while (!finishParsing);

    return result;
}

/**
 * Execute parser: Parse XML Name item
 *
 * \retval Result_NeedMoreData  More data is needed to complete the parsing session
 * \retval Result_Success       Valid XML Name found
 * \retval Result_Error         Error occurred (invalid character read)
 */
XmlItemParser::Result XmlItemParser::parseName()
{
    Result result = Result_Error;
    bool finishParsing = false;

    do
    {
        // Check if another character should be read
        if (m_parsingBuffer.size() == m_position)
        {
            if (m_xmlDataBuffer.empty())
            {
                // Data buffer is empty, wait for more data
                result = Result_NeedMoreData;
                finishParsing = true;
            }
            else
            {
                // Read next character
                const char value = m_xmlDataBuffer.read();
                m_parsingBuffer.append(1U, value);
            }
        }

        if (!finishParsing)
        {
            // Get unicode character
            uint32_t unicodeCharacter = 0U;
            size_t nextPosition = 0U;
            Utf::Result utfResult = Utf::unicodeCharacterFromUtf8(m_parsingBuffer,
                                                                  m_position,
                                                                  &nextPosition,
                                                                  &unicodeCharacter);

            switch (utfResult)
            {
                case Utf::Result_Success:
                {
                    // Check for start of Name
                    if (m_position == 0U)
                    {
                        if (XmlValidator::isNameStartChar(unicodeCharacter))
                        {
                            // Valid NameStartChar found
                            m_position = nextPosition;
                        }
                        else
                        {
                            // Error, invalid character read
                            clearParsingBuffer();
                            m_terminationCharacter = unicodeCharacter;
                            finishParsing = true;
                        }
                    }
                    // Check for rest of the Name
                    else
                    {
                        if (XmlValidator::isNameChar(unicodeCharacter))
                        {
                            // Valid NameChar found
                            m_position = nextPosition;
                        }
                        else
                        {
                            // End of Name
                            m_value = m_parsingBuffer.substr(0U, m_position);
                            clearParsingBuffer();
                            m_terminationCharacter = unicodeCharacter;
                            result = Result_Success;
                            finishParsing = true;
                        }
                    }
                    break;
                }

                case Utf::Result_Incomplete:
                {
                    // Read next character
                    break;
                }

                default:
                {
                    // Error, invalid unicode character
                    finishParsing = true;
                    break;
                }
            }
        }
    }
    while (!finishParsing);

    return result;
}

/**
 * Get parsed Value string
 *
 * \return Parsed Value string
 *
 * \note The context of this value depends on the item parser that was used
 */
std::string XmlItemParser::getValue() const
{
    return m_value;
}

/**
 * Get parsed item type
 *
 * \return Parsed item type
 *
 * \note Item type is only set in the "item type" parser
 */
XmlItemParser::ItemType XmlItemParser::getItemType() const
{
    return m_itemType;
}

/**
 * Get termination character
 *
 * \return Termination character
 *
 * A "termination character" is a unicode character that was the reason for successful completion of
 * the parsing session.
 */
uint32_t XmlItemParser::getTerminationCharacter()
{
    return m_terminationCharacter;
}

/**
 * Clear parsing buffer
 */
void XmlItemParser::clearParsingBuffer()
{
    m_parsingBuffer.clear();
    m_position = 0U;
}
