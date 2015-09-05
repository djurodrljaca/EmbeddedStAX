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
 * * amp:  &
 * * lt:   <
 * * gt:   >
 * * apos: '
 * * quot: "
 */
static std::string escapeSpecialCharacter(const char specialCharacter)
{
    std::string name;
    name.reserve(6);

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
        // characters for the escaped string, we then get size of 10 characters (2 + 7 + 1) and an
        // additional character for '\0' makes it 11 characters.
        char str[11] = { '\0' };
        const char formatString[] = "&#%" PRIu32 ";";

        const int32_t returnValue = snprintf(str, sizeof(str), formatString, unicodeCharacter);

        // Check if expected number of characters were written to str
        if ((3 < returnValue) && (returnValue < sizeof(str)))
        {
            escapedString = str;
        }
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
        // characters (3 + 6 + 1) and an additional character for '\0' makes it 11 characters.
        char str[11] = { '\0' };
        const char formatString[] = "&#x%" PRIX32 ";";

        const int32_t returnValue = snprintf(str, sizeof(str), formatString, unicodeCharacter);

        // Check if expected number of characters were written to str
        if ((3 < returnValue) && (returnValue < sizeof(str)))
        {
            escapedString = str;
        }
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
