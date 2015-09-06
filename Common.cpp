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

#include "Common.h"
#include "Utf.h"
#include <inttypes.h>
#include <stdio.h>

using namespace MiniSaxCpp;

static std::string escapeSpecialCharacter(const char specialCharacter);
static std::string escapeCharacterDecimal(const uint32_t unicodeCharacter);
static std::string escapeCharacterHexadecimal(const uint32_t unicodeCharacter);

/**
 * Escape special character
 *
 * \param specialCharacter  Special character
 *
 * \return Escaped special character string
 * \return Empty string on error
 *
 * Special characters are:
 *  - amp:  &
 *  - lt:   <
 *  - gt:   >
 *  - apos: '
 *  - quot: "
 */
static std::string escapeSpecialCharacter(const char specialCharacter)
{
    std::string name;
    name.reserve(6U);

    switch (specialCharacter)
    {
        case '&':
        {
            name = "amp";
            break;
        }

        case '<':
        {
            name = "lt";
            break;
        }

        case '>':
        {
            name = "gt";
            break;
        }

        case '\'':
        {
            name = "apos";
            break;
        }

        case '"':
        {
            name = "quot";
            break;
        }

        default:
        {
            // Error, not a special character
            break;
        }
    }

    std::string escapedString;

    if (!name.empty())
    {
        escapedString.append("&").append(name).append(";");
    }

    return escapedString;
}

/**
 * Escape a character in decimal format
 *
 * \param unicodeCharacter  Unicode character
 *
 * \return Escaped unicode character string
 * \return Empty string on error
 */
static std::string escapeCharacterDecimal(const uint32_t unicodeCharacter)
{
    std::string escapedString;

    if (Utf::isUnicodeCharacterValid(unicodeCharacter))
    {
        // The maximal allowed value unicode value encoded in UTF-8 is "0x10FFFF". This is
        // equivalent to "1114111" in decimal format. From that we can see that max string size to
        // represent the max UTF-8 value in a string is 7. If we add also the other needed
        // characters for the escaped string, we then get size of 10 characters (2 + 7 + 1).
        escapedString.reserve(10U);
        escapedString.append("&#");

        if (unicodeCharacter == 0U)
        {
            // No need to do all of the calculation below if we already know that the value is zero
            escapedString.append(1U, '0');
        }
        else
        {
            // Nonzero value
            uint32_t value = unicodeCharacter;
            uint32_t limit = 1000000U;
            bool leadingZero = true;

            do
            {
                // Get digit value
                uint32_t digitValue = 0U;

                if (value >= limit)
                {
                    digitValue = value / limit;
                    leadingZero = false;
                }

                // Add digit value to string, ignoring leading zeros
                if (!leadingZero)
                {
                    const char digit = (char)digitValue + '0';
                    escapedString.append(1U, digit);
                }

                // Recalculate the values for the next digit
                value = value % limit;
                limit = limit / 10U;
            }
            while (limit > 0U);
        }

        escapedString.append(1U, ';');
    }

    return escapedString;
}

/**
 * Escape a character in hexadecimal format
 *
 * \param unicodeCharacter  Unicode character
 *
 * \return Escaped unicode character string
 * \return Empty string on error
 */
static std::string escapeCharacterHexadecimal(const uint32_t unicodeCharacter)
{
    std::string escapedString;

    if (Utf::isUnicodeCharacterValid(unicodeCharacter))
    {
        // The maximal allowed value unicode value encoded in UTF-8 is "0x10FFFF". From that we can
        // see that max string size to represent the max UTF-8 value in a string is 6. If we add
        // also the other needed characters for the escaped string, we then get size of 10
        // characters (3 + 6 + 1).
        escapedString.reserve(10U);
        escapedString.append("&#x");

        if (unicodeCharacter == 0U)
        {
            // No need to do all of the calculation below if we already know that the value is zero
            escapedString.append(1U, '0');
        }
        else
        {
            // Nonzero value
            bool leadingZero = true;

            for (uint32_t digitIndex = 6U; digitIndex > 0U; digitIndex--)
            {
                const uint32_t shiftFactor = (digitIndex - 1U) * 4U;
                const uint32_t digitValue = (unicodeCharacter >> shiftFactor) & 0x0FU;
                char digit;

                if (digitValue == 0U)
                {
                    digit = '0';
                }
                else
                {
                    leadingZero = false;

                    if (digitValue <= 9U)
                    {
                        // Characters: '1' to '9'
                        digit = (char)digitValue + '0';
                    }
                    else
                    {
                        // Characters: 'A' to 'F'
                        digit = (char)(digitValue - 0x0AU) + 'A';
                    }
                }

                if (!leadingZero)
                {
                    escapedString.append(1U, digit);
                }
            }
        }

        escapedString.append(1U, ';');
    }

    return escapedString;
}

/**
 * Create an escaped AttValue string
 *
 * \param attributeValue    Unescaped UTF-8 encoded AttValue string
 * \param quotationMark     Quotation mark used when setting the attribute's value
 *
 * \return Escaped AttValue string
 * \return Empty string on error
 */
std::string Common::escapeAttributeValueString(const std::string &attributeValue,
                                               const Common::QuotationMark quotationMark)
{
    std::string escapedString;

    if (!attributeValue.empty())
    {
        escapedString.reserve(attributeValue.size());  // Minimize allocations

        bool characterEscaped = false;
        bool success = false;
        uint32_t unicodeCharacter = 0U;
        size_t currentPosition = 0U;
        size_t nextPosition = 0U;

        do
        {
            Utf::Result result = Utf::unicodeCharacterFromUtf8(attributeValue,
                                                               currentPosition,
                                                               &nextPosition,
                                                               &unicodeCharacter);

            success = false;

            if (result == Utf::Result_Success)
            {
                // Check if character has to be escaped
                bool escapeCharacter = false;

                switch (unicodeCharacter)
                {
                    case (uint32_t)'<':
                    case (uint32_t)'&':
                    {
                        escapeCharacter = true;
                        break;
                    }

                    case (uint32_t)'"':
                    {
                        if (quotationMark == Common::QuotationMark_Quote)
                        {
                            escapeCharacter = true;
                        }

                        break;
                    }

                    case (uint32_t)'\'':
                    {
                        if (quotationMark == Common::QuotationMark_Apostrophe)
                        {
                            escapeCharacter = true;
                        }

                        break;
                    }

                    default:
                    {
                        break;
                    }
                }

                if (escapeCharacter)
                {
                    // Escape character
                    const std::string escapedSpecialCharacter =
                            escapeSpecialCharacter(unicodeCharacter);

                    if (!escapedSpecialCharacter.empty())
                    {
                        if (!characterEscaped)
                        {
                            // Optimization: This is the first character in the string that has to
                            //               be escaped. Copy the unescaped string up to (but not
                            //               including) the current character to the escaped string.
                            const size_t len = currentPosition;

                            if (len > 0U)
                            {
                                escapedString.append(attributeValue.substr(0U, len));
                            }

                            characterEscaped = true;
                        }

                        escapedString.append(escapedSpecialCharacter);
                        success = true;
                    }
                }
                else
                {
                    // Do not escape character, just copy it
                    //
                    // Optimization: No need to copy characters if no character was escaped up to
                    //               this position. If no character is escaped in the complete
                    //               string then code below will copy the complete unescaped string
                    //               to the escaped string.
                    if (characterEscaped)
                    {
                        const size_t len = nextPosition - currentPosition;

                        if (len == 1U)
                        {
                            // Single byte unicode character
                            escapedString.append(1U, attributeValue.at(currentPosition));
                        }
                        else
                        {
                            // Multibyte unicode character
                            escapedString.append(attributeValue.substr(currentPosition, len));
                        }
                    }

                    success = true;
                }

                if (success)
                {
                    currentPosition = nextPosition;
                }
            }
        }
        while (success && (nextPosition < attributeValue.size()));

        if (success)
        {
            // Optimization: If no character was escaped, then we need to copy the complete
            //               unescaped string to the escaped string
            if (!characterEscaped)
            {
                escapedString = attributeValue;
            }
        }
        else
        {
            // On error make sure that the escapedString is empty
            if (!escapedString.empty())
            {
                escapedString.clear();
            }
        }
    }

    return escapedString;
}

/**
 * Create an escaped TextNode string
 *
 * \param textNode  Unescaped UTF-8 encoded AttValue string
 *
 * \return Escaped TextNode string
 * \return Empty string on error
 */
std::string Common::escapeTextNodeString(const std::string &textNode)
{
    std::string escapedString;

    if (!textNode.empty())
    {
        escapedString.reserve(textNode.size());  // Minimize allocations

        bool characterEscaped = false;
        bool success = false;
        uint32_t unicodeCharacter = 0U;
        size_t currentPosition = 0U;
        size_t nextPosition = 0U;

        do
        {
            Utf::Result result = Utf::unicodeCharacterFromUtf8(textNode,
                                                               currentPosition,
                                                               &nextPosition,
                                                               &unicodeCharacter);

            success = false;

            if (result == Utf::Result_Success)
            {
                // Check if character has to be escaped
                bool escapeCharacter = false;

                switch (unicodeCharacter)
                {
                    case (uint32_t)'<':
                    case (uint32_t)'&':
                    {
                        escapeCharacter = true;
                        break;
                    }

                    case (uint32_t)'>':
                    {
                        // On "]]>" sequence the '>' character needs to be escaped
                        if (currentPosition >= 2U)
                        {
                            if (textNode.substr(currentPosition - 2U, 3U) == std::string("]]>"))
                            {
                                escapeCharacter = true;
                            }
                        }

                        break;
                    }

                    default:
                    {
                        break;
                    }
                }

                if (escapeCharacter)
                {
                    // Escape character
                    const std::string escapedSpecialCharacter =
                            escapeSpecialCharacter(unicodeCharacter);

                    if (!escapedSpecialCharacter.empty())
                    {
                        if (!characterEscaped)
                        {
                            // Optimization: This is the first character in the string that has to
                            //               be escaped. Copy the unescaped string up to (but not
                            //               including) the current character to the escaped string.
                            const size_t len = currentPosition;

                            if (len > 0U)
                            {
                                escapedString.append(textNode.substr(0U, len));
                            }

                            characterEscaped = true;
                        }

                        escapedString.append(escapedSpecialCharacter);
                        success = true;
                    }
                }
                else
                {
                    // Do not escape character, just copy it
                    //
                    // Optimization: No need to copy characters if no character was escaped up to
                    //               this position. If no character is escaped in the complete
                    //               string then code below will copy the complete unescaped string
                    //               to the escaped string.
                    if (characterEscaped)
                    {
                        const size_t len = nextPosition - currentPosition;

                        if (len == 1U)
                        {
                            // Single byte unicode character
                            escapedString.append(1U, textNode.at(currentPosition));
                        }
                        else
                        {
                            // Multibyte unicode character
                            escapedString.append(textNode.substr(currentPosition, len));
                        }
                    }

                    success = true;
                }

                if (success)
                {
                    currentPosition = nextPosition;
                }
            }
        }

        while (success && (nextPosition < textNode.size()));

        if (success)
        {
            // Optimization: If no character was escaped, then we need to copy the complete
            //               unescaped string to the escaped string
            if (!characterEscaped)
            {
                escapedString = textNode;
            }
        }
        else
        {
            // On error make sure that the escapedString is empty
            if (!escapedString.empty())
            {
                escapedString.clear();
            }
        }
    }

    return escapedString;
}

/**
 * Constructor for DataBuffer
 *
 * \param size  Data buffer's size in (in bytes)
 */
Common::DataBuffer::DataBuffer(const size_t size)
    : m_dataBuffer(new char[size + 1U]),
      m_dataBufferSize(size + 1U),
      m_writePosition(0U),
      m_readPosition(0U)
{
}

/**
 * Destructor for DataBuffer
 */
Common::DataBuffer::~DataBuffer()
{
    if (m_dataBuffer != NULL)
    {
        delete[] m_dataBuffer;
    }
}

/**
 * Clear the buffer
 */
void Common::DataBuffer::clear()
{
    m_writePosition = 0U;
    m_readPosition = 0U;
}

/**
 * Checks if data buffer is empty
 *
 * \retval true     Empty
 * \retval false    Not empty
 */
bool Common::DataBuffer::empty() const
{
    bool emptyFlag = false;

    if (m_dataBufferSize < 2U)
    {
        // If there are no bytes or if there is a single byte in the data buffer then the buffer
        // cannot accept any data - data buffer is always full
    }
    else
    {
        if (m_writePosition == m_readPosition)
        {
            emptyFlag = true;
        }
    }

    return emptyFlag;
}

/**
 * Checks if data buffer is full
 *
 * \retval true     Full
 * \retval false    Not full
 */
bool Common::DataBuffer::full() const
{
    bool fullFlag = false;

    if (m_dataBufferSize < 2U)
    {
        // If there are no bytes or if there is a single byte in the data buffer then the buffer
        // cannot accept any data - data buffer is always full
        fullFlag = true;
    }
    else
    {
        // If read buffer is bigger than the write buffer by one then the data buffer is full
        if (m_readPosition == 0U)
        {
            if (m_writePosition == (m_dataBufferSize - 1U))
            {
                fullFlag = true;
            }
        }
        else if (m_readPosition == (m_writePosition + 1U))
        {
            fullFlag = true;
        }
    }

    return fullFlag;
}

/**
 * Checks how many free bytes are in the data buffer
 *
 * \return Number of free bytes
 */
size_t Common::DataBuffer::free() const
{
    size_t freeBytes;

    if (m_dataBufferSize < 2U)
    {
        // If there are no bytes or if there is a single byte in the data buffer then the buffer
        // cannot accept any data - data buffer is always full
        freeBytes = 0U;
    }
    else
    {
        if (m_readPosition > m_writePosition)
        {
            freeBytes = m_readPosition - m_writePosition - 1U;
        }
        else
        {
            freeBytes = (m_dataBufferSize - 1U - m_writePosition) + m_readPosition;
        }
    }

    return freeBytes;
}

/**
 * Checks how many bytes are used in the data buffer
 *
 * \return Number of used bytes
 */
size_t Common::DataBuffer::used() const
{
    size_t usedBytes;

    if (m_dataBufferSize < 2U)
    {
        // If there are no bytes or if there is a single byte in the data buffer then the buffer
        // cannot accept any data - data buffer is always full
        usedBytes = 0U;
    }
    else
    {
        if (m_writePosition >= m_readPosition)
        {
            usedBytes = m_writePosition - m_readPosition;
        }
        else
        {
            usedBytes = (m_dataBufferSize - 1U - m_readPosition) + m_writePosition;
        }
    }

    return usedBytes;
}

/**
 * Write data to the data buffer
 *
 * \param data  Data to write
 *
 * \retval true     Success
 * \retval false    Error, data buffer is full
 */
bool Common::DataBuffer::write(const char data)
{
    bool success = false;

    if (!full())
    {
        m_dataBuffer[m_writePosition] = data;
        m_writePosition++;
        success = true;

        // Check for overflow of the write position
        if (m_writePosition >= m_dataBufferSize)
        {
            m_writePosition = 0U;
        }
    }

    return success;
}

/**
 * Read a character from the data buffer
 *
 * \param[out] success  Optional output parameter for signaling success or failure
 *
 * \return Oldest character from the data buffer
 */
char Common::DataBuffer::read(bool *success)
{
    bool dataRead = false;
    char data = 0U;

    if (!empty())
    {
        data = m_dataBuffer[m_readPosition];
        m_readPosition++;
        dataRead = true;

        // Check for overflow of the read position
        if (m_readPosition >= m_dataBufferSize)
        {
            m_readPosition = 0U;
        }
    }

    if (success != NULL)
    {
        *success = dataRead;
    }

    return data;
}
