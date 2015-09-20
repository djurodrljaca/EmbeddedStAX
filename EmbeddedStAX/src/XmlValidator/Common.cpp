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

#include <EmbeddedStAX/XmlValidator/Common.h>

using namespace EmbeddedStAX;

/**
 * Check if character is a "whitespace" character
 *
 * \param character Unicode character
 *
 * \retval true     Character is a whitespace character
 * \retval false    Character is not a whitespace character
 *
 * Allowed values for NameStartChar:
 *  - 0x09 (horizontal tav)
 *  - 0x0A (line feed/new line)
 *  - 0x0D (carrage return)
 *  - 0x20 (space)
 */
bool XmlValidator::isWhitespace(const uint32_t character)
{
    bool valid = false;

    switch (character)
    {
        case 0x09U:
        case 0x0AU:
        case 0x0DU:
        case 0x20U:
        {
            // Whitespace character
            valid = true;
            break;
        }

        default:
        {
            // Error, invalid value
            break;
        }
    }

    return valid;
}

/**
 * This function can be used to validate a "Char"
 *
 * \param character Unicode character
 *
 * \retval true     Char is valid
 * \retval false    Char is not valid
 *
 * Allowed values for Char:
 *  - 0x9
 *  - 0xA
 *  - 0xD
 *  - [0x20 - 0xD7FF]
 *  - [0xE000 - 0xFFFD]
 *  - [0x10000 - 0x10FFFF]
 */
bool XmlValidator::isChar(const uint32_t character)
{
    bool valid = false;

    if (character == 0x09U)
    {
        valid = true;
    }
    else if (character == 0x0AU)
    {
        valid = true;
    }
    else if (character == 0x0DU)
    {
        valid = true;
    }
    else if ((0x20U <= character) && (character <= 0xD7FFU))
    {
        valid = true;
    }
    else if ((0xE000U <= character) && (character <= 0xFFFDU))
    {
        valid = true;
    }
    else if ((0x10000U <= character) && (character <= 0x10FFFFU))
    {
        valid = true;
    }
    else
    {
        // Error, invalid value
    }

    return valid;
}
