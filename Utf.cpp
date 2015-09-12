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

using namespace MiniSaxCpp;

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
            m_buffer.append(1U, data);
            m_noOfBytes = noOfBytes;
            m_value = (uint32_t)(data & startMask);
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
