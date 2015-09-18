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

using namespace EmbeddedStAX::Common;

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
    m_index = 0U;
    m_char = 0U;
    m_charSize = 0U;
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

    if (m_index == 0U)
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
uint32_t Utf8::getChar() const
{
    return m_char;
}

/**
 * Convert unicode character to UTF-8 string
 *
 * \param unicodeCharacter  Unicode character to convert
 *
 * \return UTF-8 encoded unicode character or an empty string in case of an error
 */
std::string Utf8::toUtf8(const uint32_t unicodeChar)
{
    char utf8[4];
    size_t size = 0U;

    if (unicodeChar <= 0x7FU)
    {
        // 1 byte UTF-8 character
        size = 1U;
        utf8[0] = static_cast<char>(unicodeChar);
    }
    else if (unicodeChar <= 0x7FFU)
    {
        // 2 byte UTF-8 character
        size = 2U;

        uint32_t value = unicodeChar & 0x3FU;
        value = value | 0x80U;
        utf8[1] = static_cast<char>(value);

        value = unicodeChar >> 6;
        value = value | 0xC0U;
        utf8[0] = static_cast<char>(value);
    }
    else if (unicodeChar <= 0xFFFFU)
    {
        // 3 byte UTF-8 character
        size = 3U;

        uint32_t ucValue = unicodeChar;
        uint32_t value = 0U;

        for (int32_t i = 0; i < 2; i++)
        {
            value = ucValue & 0x3FU;
            value = value | 0x80U;
            utf8[2 - i] = static_cast<char>(value);
            ucValue = ucValue >> 6;
        }

        value = value | 0xE0U;
        utf8[0] = static_cast<char>(value);
    }
    else if (unicodeChar <= 0x10FFFFU)
    {
        // 4 byte UTF-8 character
        size = 4U;

        uint32_t ucValue = unicodeChar;
        uint32_t value = 0U;

        for (int32_t i = 0; i < 3; i++)
        {
            value = ucValue & 0x3FU;
            value = value | 0x80U;
            utf8[3 - i] = static_cast<char>(value);
            ucValue = ucValue >> 6;
        }

        value = value | 0xF0U;
        utf8[0] = static_cast<char>(value);
    }
    else
    {
        // Error, invalid unicode character
    }

    return std::string(utf8, size);
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
        m_char = static_cast<uint32_t>(data);
        m_charSize = 0U;
        m_index = 0U;
        result = Result_Success;
    }
    else
    {
        // Unicode character contains more than 1 byte
        uint8_t value = static_cast<uint8_t>(data);
        uint8_t startMask = 0U;
        size_t noOfBytes = 0U;

        // Check for 2 byte UTF-8 start character
        if ((value & 0xE0U) == 0xC0U)
        {
            noOfBytes = 2U;
            startMask = static_cast<uint8_t>(~0xE0U);
        }
        // Check for 3 byte UTF-8 start character
        else if ((value & 0xF0U) == 0xE0U)
        {
            noOfBytes = 3U;
            startMask = static_cast<uint8_t>(~0xF0U);
        }
        // Check for 4 byte UTF-8 start character
        else if ((value & 0xF8U) == 0xF0U)
        {
            noOfBytes = 4U;
            startMask = static_cast<uint8_t>(~0xF8U);
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
            m_char = static_cast<uint32_t>(value & startMask);
            m_charSize = noOfBytes;
            m_index = 1U;
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

    if (m_index < m_charSize)
    {
        // Parse data
        uint32_t value = static_cast<uint32_t>(data);

        if ((value & 0xC0U) == 0x80U)
        {
            // Partial value is valid, remove "no of bytes" code from the partial value, shift
            // unicode character value to the left and add the partial value to it
            value = value & 0x3FU;
            m_char = (m_char << 6U) | value;

            // Check for the last byte of a unicode character
            m_index++;

            if (m_index == m_charSize)
            {
                // Last byte of a unicode character found, check if the unicode character value is
                // valid
                if (m_char <= 0x10FFFFU)
                {
                    m_charSize = 0U;
                    m_index = 0U;
                    result = Result_Success;
                }
                else
                {
                    // Error, invalid unicode value
                    clear();
                }
            }
            else if (m_index > m_charSize)
            {
                // Error, invalid index (this part of code should never be reached!)
                clear();
            }
            else
            {
                // More data is needed
                result = Result_Incomplete;
            }
        }
        else
        {
            // Error, invalid value
            clear();
        }
    }
    else
    {
        // Error, invalid size (this part of code should never be reached!)
        clear();
    }

    return result;
}
