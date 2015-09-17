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

#include "Utf.h"

using namespace EmbeddedStAX;

/**
 * Constructor for Utf8
 */
Utf8::Utf8()
{
    clear();
}

/**
 * Clear Utf8 object
 */
void Utf8::clear()
{
    m_buffer.clear();
    m_noOfBytes = 0U;
    m_value = 0U;
    m_utf8.clear();
}

/**
 * Write data
 *
 * \param data  Data to write
 *
 * \retval Result_Success       Successfully converted the UTF-8 encoded string to a unicode
 *                              character
 * \retval Result_Incomplete    Incomplete unicode character (more data is needed)
 * \retval Result_Error         Error
 */
Utf8::Result Utf8::write(const char data)
{
    Result result = Result_Error;

    if (m_buffer.empty())
    {
        result = writeFirstCharacter(data);
    }
    else
    {
        result = writeNextCharacter(data);
    }

    return result;
}

/**
 * Get unicode character
 *
 * \return Unicode character
 */
uint32_t Utf8::getUnicodeCharacter() const
{
    return m_value;
}

/**
 * Convert unicode character to UTF-8 string
 *
 * \param unicodeCharacter  Unicode character to convert
 *
 * \return UTF-8 encoded unicode character or an empty string in case of an error
 */
std::string Utf8::toUtf8(const uint32_t unicodeCharacter)
{
    std::string utf8;

    if (unicodeCharacter <= 0x7FU)
    {
        // 1 byte UTF-8 character
        utf8.append(1U, (char)unicodeCharacter);
    }
    else if (unicodeCharacter <= 0x7FFU)
    {
        // 2 byte UTF-8 character
        char data[2];

        uint32_t value = unicodeCharacter & 0x3FU;
        value = value | 0x80U;
        data[1] = (char)value;

        value = unicodeCharacter >> 6;
        value = value | 0xC0U;
        data[0] = (char)value;

        utf8.append(data, 2U);
    }
    else if (unicodeCharacter <= 0xFFFFU)
    {
        // 3 byte UTF-8 character
        uint32_t ucValue = unicodeCharacter;
        char data[3];
        uint32_t value = 0U;

        for (int32_t i = 0; i < 2; i++)
        {
            value = ucValue & 0x3FU;
            value = value | 0x80U;
            data[2 - i] = (char)value;
            ucValue = ucValue >> 6;
        }

        value = value | 0xE0U;
        data[0] = (char)value;

        utf8.append(data, 3U);
    }
    else if (unicodeCharacter <= 0x10FFFFU)
    {
        // 4 byte UTF-8 character
        uint32_t ucValue = unicodeCharacter;
        char data[4];
        uint32_t value = 0U;

        for (int32_t i = 0; i < 3; i++)
        {
            value = ucValue & 0x3FU;
            value = value | 0x80U;
            data[3 - i] = (char)value;
            ucValue = ucValue >> 6;
        }

        value = value | 0xF0U;
        data[0] = (char)value;

        utf8.append(data, 4U);
    }
    else
    {
        // Error, invalid unicode character
    }

    return utf8;
}

/**
 * Convert unicode string to UTF-8 string
 *
 * \param unicodeString String of unicode characters
 *
 * \return UTF-8 encoded unicode string or an empty string in case of an error
 */
std::string Utf8::toUtf8(const UnicodeString &unicodeString)
{
    std::string utf8String;

    for (size_t i = 0U; i < unicodeString.size(); i++)
    {
        const std::string utf8Character = Utf8::toUtf8(unicodeString.at(i));

        if (utf8Character.empty())
        {
            utf8String.clear();
            break;
        }
        else
        {
            utf8String.append(utf8Character);
        }
    }

    return utf8String;
}

/**
 * Write first character
 *
 * \param data  Data to write
 *
 * \return Unicode character encoded in UTF-8
 *
 * \retval Result_Success       Successfully converted the UTF-8 encoded string to a unicode
 *                              character
 * \retval Result_Incomplete    Incomplete unicode character (more data is needed)
 * \retval Result_Error         Error, invalid UTF-8 start character
 */
Utf8::Result Utf8::writeFirstCharacter(const char data)
{
    Result result = Result_Error;

    if (data < 0x80U)
    {
        // Unicode character contains only 1 byte, its value was successfully converted from the
        // utf8 encoded character to an unicode character
        m_noOfBytes = 0U;
        m_value = (uint32_t)data;
        m_utf8 = std::string(1U, data);
        result = Result_Success;
    }
    else
    {
        // Unicode character contains more than 1 byte
        uint32_t value = (uint32_t)data;
        uint32_t startMask = 0U;
        size_t noOfBytes = 0U;

        // Check for 2 byte UTF-8 start character
        if ((value & 0xE0U) == 0xC0U)
        {
            noOfBytes = 2U;
            startMask = (uint32_t)(~0xE0U);
        }
        // Check for 3 byte UTF-8 start character
        else if ((value & 0xF0U) == 0xE0U)
        {
            noOfBytes = 3U;
            startMask = (uint32_t)(~0xF0U);
        }
        // Check for 4 byte UTF-8 start character
        else if ((value & 0xF8U) == 0xF0U)
        {
            noOfBytes = 4U;
            startMask = (uint32_t)(~0xF8U);
        }
        else
        {
            // Error, invalid UTF-8 start character
            clear();
        }

        if ((noOfBytes > 0U) &&
            (startMask > 0U))
        {
            // Add the partial value to the unicode character (without "no. of bytes" code)
            m_buffer.append(1U, value);
            m_noOfBytes = noOfBytes;
            m_value = value & startMask;
            m_utf8.clear();
            result = Result_Incomplete;
        }
    }

    return result;
}

/**
 * Write next character
 *
 * \param data  Data to write
 *
 * \return Unicode character encoded in UTF-8
 *
 * \retval Result_Success       Successfully converted the UTF-8 encoded string to a unicode
 *                              character
 * \retval Result_Incomplete    Incomplete unicode character (more data is needed)
 * \retval Result_Error         Error, invalid UTF-8 start character
 */
Utf8::Result Utf8::writeNextCharacter(const char data)
{
    Result result = Result_Error;
    const size_t size = m_buffer.size();

    if (size < m_noOfBytes)
    {
        // Parse data
        uint32_t partialValue = (uint32_t)data;

        if ((partialValue & 0xC0U) == 0x80U)
        {
            // Partial value is valid, remove "no of bytes" code from the partial value, shift
            // unicode character value to the left and add the partial value to it
            partialValue = partialValue & 0x3FU;
            m_value = (uint32_t)(m_value << 6U);
            m_value = m_value | partialValue;

            // Add data to the buffer and check if was the last byte of a unicode character
            m_buffer.append(1U, data);

            if (m_buffer.size() == m_noOfBytes)
            {
                // Last byte of a unicode character found, check if the unicode character value is
                // valid
                if (m_value <= 0x10FFFFU)
                {
                    m_noOfBytes = 0U;
                    m_utf8 = m_buffer;
                    m_buffer.clear();
                    result = Result_Success;
                }
                else
                {
                    // Error, invalid unicode value
                    clear();
                }
            }
            else
            {
                // More data is needed
                result = Result_Incomplete;
            }
        }
        else
        {
            // Error, invalid partial value
            clear();

            // Try to synchronize to the next character (if data is a valid start value)
            result = writeFirstCharacter(data);
        }
    }
    else
    {
        // Error, invalid size (this part of code should never be reached!)
        clear();
    }

    return result;
}

// -------------------------------------------------------------------------------------------------
// TODO: merge this below with Utf8 ?

/**
 * Convert a character from the UTF-8 encoded string at specified position to a unicode character.
 *
 * \param      utf8                     UTF-8 encoded string
 * \param      startPosition            Start position in the utf8 string
 * \param[out] nextCharacterPosition    Position of the next unicode character in the utf8 string
 * \param[out] unicodeCharacter         Value of the unicode character
 *
 * \retval Result_Success       Success
 * \retval Result_Incomplete    Not enough bytes in utf8 string to fully read the unicode character
 * \retval Result_Error         Error
 */
Utf::Result Utf::unicodeCharacterFromUtf8(const std::string &utf8,
                                          const size_t startPosition,
                                          size_t *nextCharacterPosition,
                                          uint32_t *unicodeCharacter)
{
    Result result = Result_Error;

    if ((utf8.size() > startPosition) &&
        (nextCharacterPosition != NULL) &&
        (unicodeCharacter != NULL))
    {
        // Read the value of the unicode character
        uint32_t value = (uint32_t)utf8.at(startPosition);

        if (value < 0x80U)
        {
            // Unicode character contains only 1 byte, its value was successfully read from the utf8
            // string. Save unicode character value and position of the next unicode character.
            *unicodeCharacter = value;
            *nextCharacterPosition = startPosition + 1U;
            result = Result_Success;
        }
        else
        {
            // Unicode character contains more than 1 byte
            uint32_t startMask = 0U;
            uint32_t noOfBytes = 0U;

            // Check for 2 byte UTF-8 start character
            if ((value & 0xE0U) == 0xC0U)
            {
                noOfBytes = 2U;
                startMask = (uint32_t)(~0xE0U);
            }
            // Check for 3 byte UTF-8 start character
            else if ((value & 0xF0U) == 0xE0U)
            {
                noOfBytes = 3U;
                startMask = (uint32_t)(~0xF0U);
            }
            // Check for 4 byte UTF-8 start character
            else if ((value & 0xF8U) == 0xF0U)
            {
                noOfBytes = 4U;
                startMask = (uint32_t)(~0xF8U);
            }
            else
            {
                // Error, invalid UTF-8 start character
            }

            if ((noOfBytes > 0U) && (startMask > 0U))
            {
                // Check if utf8 string contains enough bytes
                const size_t minSize = startPosition + (size_t)noOfBytes;

                if (utf8.size() >= minSize)
                {
                    // Remove "no of bytes" code from the value
                    value = value & startMask;

                    // Add partial values from the other bytes and add them to the unicode character
                    bool error = false;

                    for (uint32_t byteNo = 1U; byteNo < noOfBytes; byteNo++)
                    {
                        // Get partial value and check if it is valid
                        const size_t position = startPosition + (size_t)byteNo;
                        uint32_t partialValue = (uint32_t)utf8.at(position);

                        if ((partialValue & 0xC0U) == 0x80U)
                        {
                            // Partial value is valid, remove "no of bytes" code from the partial
                            // value, shift unicode character value to the left and add the partial
                            // value to it
                            partialValue = partialValue & 0x3FU;
                            value = (uint32_t)(value << 6U);
                            value = value | partialValue;
                        }
                        else
                        {
                            // Error, invalid partial value
                            error = true;
                            break;
                        }
                    }

                    if (!error)
                    {
                        // Unicode character's value was successfully read from the utf8 string
                        if (isUnicodeCharacterValid(value))
                        {
                            *unicodeCharacter = value;
                            *nextCharacterPosition = minSize;
                            result = Result_Success;
                        }
                    }
                }
                else
                {
                    // Not enough data in the utf8 string
                    result = Result_Incomplete;
                }
            }
        }
    }

    return result;
}

/**
 * Check if unicode character is valid
 *
 * \param unicodeCharacter  Unicode character
 *
 * \retval true     Valid unicode character
 * \retval false    Invalid unicode character
 */
bool Utf::isUnicodeCharacterValid(const uint32_t unicodeCharacter)
{
    bool valid = false;

    if (unicodeCharacter <= 0x10FFFF)
    {
        valid = true;
    }

    return valid;
}
