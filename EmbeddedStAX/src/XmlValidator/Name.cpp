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

#include <EmbeddedStAX/XmlValidator/Name.h>

using namespace EmbeddedStAX;

/**
 * Check if character is a "NameStartChar" character
 *
 * \param character Unicode character
 *
 * \retval true     NameStartChar is valid
 * \retval false    NameStartChar is not valid
 *
 * Allowed values for NameStartChar:
 *  - ':'
 *  - [A - Z]
 *  - '_'
 *  - [a - z]
 *  - [0xC0 - 0xD6]
 *  - [0xD8 - 0xF6]
 *  - [0xF8 - 0x2FF]
 *  - [0x370 - 0x37D]
 *  - [0x37F - 0x1FFF]
 *  - [0x200C - 0x200D]
 *  - [0x2070 - 0x218F]
 *  - [0x2C00 - 0x2FEF]
 *  - [0x3001 - 0xD7FF]
 *  - [0xF900 - 0xFDCF]
 *  - [0xFDF0 - 0xFFFD]
 *  - [0x10000 - 0xEFFFF]
 */
bool XmlValidator::isNameStartChar(const uint32_t character)
{
    bool valid = false;

    if (character == (uint32_t)':')
    {
        valid = true;
    }
    else if (((uint32_t)'A' <= character) && (character <= (uint32_t)'Z'))
    {
        valid = true;
    }
    else if (character == (uint32_t)'_')
    {
        valid = true;
    }
    else if (((uint32_t)'a' <= character) && (character <= (uint32_t)'z'))
    {
        valid = true;
    }
    else if ((0xC0U <= character) && (character <= 0xD6U))
    {
        valid = true;
    }
    else if ((0xD8U <= character) && (character <= 0xF6U))
    {
        valid = true;
    }
    else if ((0xF8U <= character) && (character <= 0x2FF0U))
    {
        valid = true;
    }
    else if ((0x370U <= character) && (character <= 0x37DU))
    {
        valid = true;
    }
    else if ((0x37FU <= character) && (character <= 0x1FFFU))
    {
        valid = true;
    }
    else if ((0x200CU <= character) && (character <= 0x200DU))
    {
        valid = true;
    }
    else if ((0x2070U <= character) && (character <= 0x218FU))
    {
        valid = true;
    }
    else if ((0x2C00U <= character) && (character <= 0x2FEFU))
    {
        valid = true;
    }
    else if ((0x3001U <= character) && (character <= 0xD7FFU))
    {
        valid = true;
    }
    else if ((0xF900U <= character) && (character <= 0xFDCFU))
    {
        valid = true;
    }
    else if ((0xFDF0U <= character) && (character <= 0xFFFDU))
    {
        valid = true;
    }
    else if ((0x10000U <= character) && (character <= 0xEFFFFU))
    {
        valid = true;
    }
    else
    {
        // Error, invalid value
    }

    return valid;
}

/**
 * Check if character is a "NameChar" character
 *
 * \param character Unicode character
 *
 * \retval true     NameChar is valid
 * \retval false    NameChar is not valid
 *
 * Allowed values for NameChar:
 *  - NameStartChar
 *  - '-'
 *  - '.'
 *  - [0 - 9]
 *  - 0xB7
 *  - [0x0300 - 0x036F]
 *  - [0x203F - 0x2040]
 */
bool XmlValidator::isNameChar(const uint32_t character)
{
    bool valid = XmlValidator::isNameStartChar(character);

    if (!valid)
    {
        if (character == (uint32_t)'.')
        {
            valid = true;
        }
        else if (character == (uint32_t)'-')
        {
            valid = true;
        }
        else if (((uint32_t)'0' <= character) && (character <= (uint32_t)'9'))
        {
            valid = true;
        }
        else if (character == 0xB7U)
        {
            valid = true;
        }
        else if ((0x0300U <= character) && (character <= 0x036FU))
        {
            valid = true;
        }
        else if ((0x203FU <= character) && (character <= 0x2040U))
        {
            valid = true;
        }
        else
        {
            // Error, invalid value
        }
    }

    return valid;
}
