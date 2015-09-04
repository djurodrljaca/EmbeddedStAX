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
#include "XmlValidator.h"
#include "Utf.h"

using namespace MiniSaxCpp;

static bool validateNameStartChar(const uint32_t nameStartChar);
static bool validateNameChar(const uint32_t nameChar);
static bool validateChar(const uint32_t character);

/**
 * This function can be used to validate a "NameStartChar"
 *
 * \param nameStartChar Unicode character
 *
 * \retval true     NameStartChar is valid
 * \retval false    NameStartChar is not valid
 *
 * Allowed values for NameStartChar:
 *       * ":"
 *       * [A - Z]
 *       * "_"
 *       * [a - z]
 *       * [0xC0 - 0xD6]
 *       * [0xD8 - 0xF6]
 *       * [0xF8 - 0x2FF]
 *       * [0x370 - 0x37D]
 *       * [0x37F - 0x1FFF]
 *       * [0x200C - 0x200D]
 *       * [0x2070 - 0x218F]
 *       * [0x2C00 - 0x2FEF]
 *       * [0x3001 - 0xD7FF]
 *       * [0xF900 - 0xFDCF]
 *       * [0xFDF0 - 0xFFFD]
 *       * [0x10000 - 0xEFFFF]
 */
static bool validateStartNameChar(const uint32_t nameStartChar)
{
    bool valid = false;

    if (nameStartChar == (uint32_t)':')
    {
        valid = true;
    }
    else if (((uint32_t)'A' <= nameStartChar) && (nameStartChar <= (uint32_t)'Z'))
    {
        valid = true;
    }
    else if (nameStartChar == (uint32_t)'_')
    {
        valid = true;
    }
    else if (((uint32_t)'a' <= nameStartChar) && (nameStartChar <= (uint32_t)'z'))
    {
        valid = true;
    }
    else if ((0xC0U <= nameStartChar) && (nameStartChar <= 0xD6U))
    {
        valid = true;
    }
    else if ((0xD8U <= nameStartChar) && (nameStartChar <= 0xF6U))
    {
        valid = true;
    }
    else if ((0xF8U <= nameStartChar) && (nameStartChar <= 0x2FF0U))
    {
        valid = true;
    }
    else if ((0x370U <= nameStartChar) && (nameStartChar <= 0x37DU))
    {
        valid = true;
    }
    else if ((0x37FU <= nameStartChar) && (nameStartChar <= 0x1FFFU))
    {
        valid = true;
    }
    else if ((0x200CU <= nameStartChar) && (nameStartChar <= 0x200DU))
    {
        valid = true;
    }
    else if ((0x2070U <= nameStartChar) && (nameStartChar <= 0x218FU))
    {
        valid = true;
    }
    else if ((0x2C00U <= nameStartChar) && (nameStartChar <= 0x2FEFU))
    {
        valid = true;
    }
    else if ((0x3001U <= nameStartChar) && (nameStartChar <= 0xD7FFU))
    {
        valid = true;
    }
    else if ((0xF900U <= nameStartChar) && (nameStartChar <= 0xFDCFU))
    {
        valid = true;
    }
    else if ((0xFDF0U <= nameStartChar) && (nameStartChar <= 0xFFFDU))
    {
        valid = true;
    }
    else if ((0x10000U <= nameStartChar) && (nameStartChar <= 0xEFFFFU))
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
 * This function can be used to validate a "NameChar"
 *
 * \param nameChar  Unicode character
 *
 * \retval true     NameChar is valid
 * \retval false    NameChar is not valid
 *
 * Allowed values for NameChar:
 *       * NameStartChar
 *       * "-"
 *       * "."
 *       * [0 - 9]
 *       * 0xB7
 *       * [0x0300 - 0x036F]
 *       * [0x203F - 0x2040]
 */
static bool validateNameChar(const uint32_t nameChar)
{
    bool valid = validateNameStartChar();

    if (!valid)
    {
        if (nameChar == (uint32_t)'.')
        {
            valid = true;
        }
        else if (nameChar == (uint32_t)'-')
        {
            valid = true;
        }
        else if (((uint32_t)'0' <= nameChar) && (nameChar <= (uint32_t)'9'))
        {
            valid = true;
        }
        else if (nameChar == 0xB7U)
        {
            valid = true;
        }
        else if ((0x0300U <= nameChar) && (nameChar <= 0x036FU))
        {
            valid = true;
        }
        else if ((0x203FU <= nameChar) && (nameChar <= 0x2040U))
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

/**
 * This function can be used to validate a "Char"
 *
 * \param character Unicode character
 *
 * \retval true     Char is valid
 * \retval false    Char is not valid
 *
 * Allowed values for Char:
 *       * 0x9
 *       * 0xA
 *       * 0xD
 *       * [0x20 - 0xD7FF]
 *       * [0xE000 - 0xFFFD]
 *       * [0x10000 - 0x10FFFF]
 */
static bool validateChar(const uint32_t character)
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

/**
 * This function can be used to validate a Name string
 *
 * \param name  UTF-8 encoded Name string
 *
 * \retval true     Valid
 * \retval false    Invalid
 * 
 * Valid Name string format:
 * \code{.unparsed}
 * Name ::= NameStartChar (NameChar)*
 * \endcode
 */
bool XmlValidator::validateNameString(const std::string &name)
{
    bool valid = false;

    if (!name.empty())
    {
        // Validate start character
        uint32_t unicodeCharacter = 0U;
        size_t currentPosition = 0U;
        size_t nextPosition = 0U;

        Utf::Result result = Utf::unicodeCharacterFromUtf8(
                                 name,
                                 currentPosition,
                                 &nextPosition,
                                 &unicodeCharacter);

        if (result == Utf::Result_Success)
        {
            if (validateNameStartChar(unicodeCharacter))
            {
                // NameStartChar is valid
                valid = true;
            }
        }

        // Validate the rest of the characters
        while (valid && (nextPosition < name.size()))
        {
            // Validate next character
            currentPosition = nextPosition;
            result = Utf::unicodeCharacterFromUtf8(
                         name,
                         currentPosition,
                         &nextPosition,
                         &unicodeCharacter);

            valid = false;

            if (result == Utf::Result_Success)
            {
                if (validateNameStartChar(unicodeCharacter))
                {
                    // NameStartChar is valid
                    valid = true;
                }
            }
        }
    }

    return valid;
}

/**
 * This function can be used to validate a Comment string
 *
 * \param name  UTF-8 encoded Name string
 *
 * \retval true     Valid
 * \retval false    Invalid
 * 
 * Valid Comment string format:
 * \code{.unparsed}
 * Comment ::= '<!--' ((Char - '-') | ('-' (Char - '-')))* '-->'
 * \endcode
 */
bool XmlValidator::validateCommentString(const std::string &comment)
{
    bool valid = false;

    if (comment.empty())
    {
        // Comment can be empty
        valid = true;
    }
    else
    {
        // Comment must not be a single '-' character
        if (comment != "-")
        {
            valid = true;
        }

        // Comment must not contain "--" sequence or end with a '-' character, otherwise all Char
        // characters are allowed
        if (valid)
        {
            uint32_t lastUnicodeCharacter = 0U;
            uint32_t unicodeCharacter = 0U;
            size_t currentPosition = 0U;
            size_t nextPosition = 0U;

            do
            {
                // Validate character
                Utf::Result result = Utf::unicodeCharacterFromUtf8(
                                         comment,
                                         currentPosition,
                                         &nextPosition,
                                         &unicodeCharacter);

                valid = false;

                if (result == Utf::Result_Success)
                {
                    // Check if Char character
                    if (validateChar(unicodeCharacter))
                    {
                        // Character is valid
                        valid = true;
                    }
                }
                
                if (valid)
                {
                    // Check for "--" sequence
                    if ((lastUnicodeCharacter == (uint32_t)'-') &&
                        (unicodeCharacter == (uint32_t)'-'))
                    {
                        // Error, "--" sequence detected
                        valid = false;
                    }
                }
                
                if (valid)
                {
                    // Prepare for next loop cycle
                    currentPosition = nextPosition;
                    lastUnicodeCharacter = unicodeCharacter;
                }
            }
            while (valid && (nextPosition < comment.size()));

            // Comment must not end with a '-' character
            if (valid)
            {
                if (lastUnicodeCharacter == (uint32_t)'-')
                {
                    // Error, comment ends with a '-' character
                    valid = false;
                }
            }
        }
    }

    return valid;
}
