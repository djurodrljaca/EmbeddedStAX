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
