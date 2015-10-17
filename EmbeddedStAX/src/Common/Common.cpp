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

#include <EmbeddedStAX/Common/Common.h>

using namespace EmbeddedStAX;

/**
 * Parse digit
 *
 * \param      digitCharacter   Digit character
 * \param      base             Digit's base (10 for decimal and 16 for hexadecimal)
 * \param[out] digitValue       Output for the parsed digit value
 *
 * \retval true     Success
 * \retval false    Error
 */
bool Common::parseDigit(const uint32_t digitCharacter, const uint32_t base, uint32_t *digitValue)
{
    bool success = false;

    if (digitValue != NULL)
    {
        switch (base)
        {
            case 10U:
            {
                if ((static_cast<uint32_t>('0') <= digitCharacter) &&
                    (digitCharacter <= static_cast<uint32_t>('9')))
                {
                    *digitValue = (uint32_t)(digitCharacter - static_cast<uint32_t>('0'));
                    success = true;
                }

                break;
            }

            case 16U:
            {
                if ((static_cast<uint32_t>('0') <= digitCharacter) &&
                    (digitCharacter <= static_cast<uint32_t>('9')))
                {
                    *digitValue = (uint32_t)(digitCharacter - static_cast<uint32_t>('0'));
                    success = true;
                }
                else if ((static_cast<uint32_t>('a') <= digitCharacter) &&
                         (digitCharacter <= static_cast<uint32_t>('f')))
                {
                    *digitValue = (uint32_t)(10U + (digitCharacter - static_cast<uint32_t>('a')));
                    success = true;
                }
                else if ((static_cast<uint32_t>('A') <= digitCharacter) &&
                         (digitCharacter <= static_cast<uint32_t>('F')))
                {
                    *digitValue = (uint32_t)(10U + (digitCharacter - static_cast<uint32_t>('A')));
                    success = true;
                }
                else
                {
                    // Error, invalid digit
                }

                break;
            }

            default:
            {
                // Error, invalid base
                break;
            }
        }
    }

    return success;
}
