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

#include <EmbeddedStAX/XmlValidator/Reference.h>
#include <EmbeddedStAX/Common/Common.h>
#include <EmbeddedStAX/XmlValidator/Common.h>
#include <EmbeddedStAX/XmlValidator/Name.h>

using namespace EmbeddedStAX;

/**
 * Validate a Reference
 *
 * \param      value            UTF-8 encoded string
 * \param      startPosition    Start position in the value string
 * \param[out] nextPosition     Optional output parameter for the position of the character after
 *                              the validated value
 *
 * \retval true     Valid
 * \retval false    Invalid
 *
 * Format:
 * \code{.unparsed}
 * Reference ::= EntityRef | CharRef
 * \endcode
 */
bool XmlValidator::validateReferece(const std::string &value,
                                    const size_t startPosition,
                                    size_t *nextPosition)
{
    bool valid = false;

    if (startPosition < value.size())
    {
        // Convert the UTF-8 encoded string to unicode string
        const Common::UnicodeString usValue =
                Common::Utf8::toUnicodeString(value.substr(startPosition));

        // Validate
        size_t usValueNextPosition = 0U;
        valid = validateReferece(usValue, 0U, &usValueNextPosition);

        // Calculate next position
        if (valid && (nextPosition != NULL))
        {
            const size_t utf8Size = Common::Utf8::calculateSize(usValue, 0U, usValueNextPosition);
            *nextPosition = startPosition + utf8Size;
        }
    }

    return valid;
}

/**
 * Validate a Reference
 *
 * \param      value            Unicode string
 * \param      startPosition    Start position in the value string
 * \param[out] nextPosition     Optional output parameter for the position of the character after
 *                              the validated value
 *
 * \retval true     Valid
 * \retval false    Invalid
 *
 * Format:
 * \code{.unparsed}
 * Reference ::= EntityRef | CharRef
 * \endcode
 */
bool XmlValidator::validateReferece(const Common::UnicodeString &value,
                                    const size_t startPosition,
                                    size_t *nextPosition)
{
    bool valid = false;

    if (startPosition < (value.size() + 2U))
    {
        // Check for start of a refernece
        if (value.at(0U) == static_cast<uint32_t>('&'))
        {
            // Check for a character reference
            if (value.at(1U) == static_cast<uint32_t>('#'))
            {
                valid = validateCharacterReferece(value, startPosition, nextPosition);
            }
            // Check for a entity reference
            else if (isNameStartChar(value.at(1U)))
            {
                valid = validateEntityReferece(value, startPosition, nextPosition);
            }
            else
            {
                // Error, not a valid reference
            }
        }
    }

    return valid;
}

/**
 * Validate a Entity Reference
 *
 * \param      value            UTF-8 encoded string
 * \param      startPosition    Start position in the value string
 * \param[out] nextPosition     Optional output parameter for the position of the character after
 *                              the validated value
 *
 * \retval true     Valid
 * \retval false    Invalid
 *
 * Format:
 * \code{.unparsed}
 * EntityRef ::= '&' Name ';'
 * \endcode
 */
bool XmlValidator::validateEntityReferece(const std::string &value,
                                          const size_t startPosition,
                                          size_t *nextPosition)
{
    bool valid = false;

    if (startPosition < value.size())
    {
        // Convert the UTF-8 encoded string to unicode string
        const Common::UnicodeString usValue =
                Common::Utf8::toUnicodeString(value.substr(startPosition));

        // Validate
        size_t usValueNextPosition = 0U;
        valid = validateEntityReferece(usValue, 0U, &usValueNextPosition);

        // Calculate next position
        if (valid && (nextPosition != NULL))
        {
            const size_t utf8Size = Common::Utf8::calculateSize(usValue, 0U, usValueNextPosition);
            *nextPosition = startPosition + utf8Size;
        }
    }

    return valid;
}

/**
 * Validate a Entity Reference
 *
 * \param      value            Unicode string
 * \param      startPosition    Start position in the value string
 * \param[out] nextPosition     Optional output parameter for the position of the character after
 *                              the validated value
 *
 * \retval true     Valid
 * \retval false    Invalid
 *
 * Format:
 * \code{.unparsed}
 * EntityRef ::= '&' Name ';'
 * \endcode
 */
bool XmlValidator::validateEntityReferece(const Common::UnicodeString &value,
                                          const size_t startPosition,
                                          size_t *nextPosition)
{
    bool valid = false;

    if (startPosition < (value.size() + 3U))
    {
        // Check for start of a refernece
        if (value.at(0U) == static_cast<uint32_t>('&'))
        {
            // Check for a start of entity reference name
            if (isNameStartChar(value.at(1U)))
            {
                // Check for the rest of entity reference name
                bool validationFinished = false;

                for (size_t i = 2U; (i < value.size()) && !validationFinished; i++)
                {
                    if (value.at(i) == static_cast<uint32_t>(';'))
                    {
                        // End of reference found
                        if (nextPosition != NULL)
                        {
                            *nextPosition = i + 1U;
                        }

                        validationFinished = true;
                        valid = true;
                    }
                    else if (isNameChar(value.at(i)))
                    {
                        // Valid name character
                    }
                    else
                    {
                        // Error, invalid character
                        validationFinished = true;
                    }
                }
            }
            else
            {
                // Error, not a valid reference
            }
        }
    }

    return valid;
}

/**
 * Validate a Character Reference
 *
 * \param      value            UTF-8 encoded string
 * \param      startPosition    Start position in the value string
 * \param[out] nextPosition     Optional output parameter for the position of the character after
 *                              the validated value
 *
 * \retval true     Valid
 * \retval false    Invalid
 *
 * Format:
 * \code{.unparsed}
 * CharRef ::= '&#' [0-9]+ ';'
            |  '&#x' [0-9a-fA-F]+ ';'
 * \endcode
 */
bool XmlValidator::validateCharacterReferece(const std::string &value,
                                             const size_t startPosition,
                                             size_t *nextPosition)
{
    bool valid = false;

    if (startPosition < value.size())
    {
        // Convert the UTF-8 encoded string to unicode string
        const Common::UnicodeString usValue =
                Common::Utf8::toUnicodeString(value.substr(startPosition));

        // Validate
        size_t usValueNextPosition = 0U;
        valid = validateCharacterReferece(usValue, 0U, &usValueNextPosition);

        // Calculate next position
        if (valid && (nextPosition != NULL))
        {
            const size_t utf8Size = Common::Utf8::calculateSize(usValue, 0U, usValueNextPosition);
            *nextPosition = startPosition + utf8Size;
        }
    }

    return valid;
}

/**
 * Validate a Character Reference
 *
 * \param      value            Unicode string
 * \param      startPosition    Start position in the value string
 * \param[out] nextPosition     Optional output parameter for the position of the character after
 *                              the validated value
 *
 * \retval true     Valid
 * \retval false    Invalid
 *
 * Format:
 * \code{.unparsed}
 * CharRef ::= '&#' [0-9]+ ';'
            |  '&#x' [0-9a-fA-F]+ ';'
 * \endcode
 */
bool XmlValidator::validateCharacterReferece(const Common::UnicodeString &value,
                                             const size_t startPosition,
                                             size_t *nextPosition)
{
    bool valid = false;

    if (startPosition < (value.size() + 4U))
    {
        // Check for start of a refernece
        if ((value.at(startPosition + 0U) == static_cast<uint32_t>('&')) &&
            (value.at(startPosition + 1U) == static_cast<uint32_t>('#')))
        {
            // Check for decimal or hexadecimal character reference
            size_t charValueStartPosition;
            uint32_t base;

            if (value.at(startPosition + 2U) == static_cast<uint32_t>('x'))
            {
                // Hexadecimal character reference
                charValueStartPosition = startPosition + 3U;
                base = 16U;
            }
            else
            {
                // Decimal character reference
                charValueStartPosition = startPosition + 2U;
                base = 10U;
            }

            // Validate character reference value
            bool validationFinished = false;
            uint32_t charValue = 0U;

            for (size_t i = charValueStartPosition; (i < value.size()) && !validationFinished; i++)
            {
                const uint32_t character = value.at(i);

                if (character == static_cast<uint32_t>(';'))
                {
                    // Check if at least one hexadecimal character was found
                    if (i > charValueStartPosition)
                    {
                        // Check if character reference value is a valid Char
                        if (XmlValidator::isChar(charValue))
                        {
                            if (nextPosition != NULL)
                            {
                                // End of character reference found
                                *nextPosition = i + 1U;
                            }

                            valid = true;
                        }
                    }

                    validationFinished = true;
                }
                else
                {
                    // Parse digit from character
                    uint32_t digitValue;
                    valid = Common::parseDigit(character, base, &digitValue);

                    if (valid)
                    {
                        // Add the digit to the character value
                        charValue = (charValue * base) + digitValue;

                        if (!Common::isUnicodeChar(charValue))
                        {
                            // Error, invalid character value
                            validationFinished = true;
                        }
                    }
                    else
                    {
                        // Error, invalid digit
                        validationFinished = true;
                    }
                }
            }
        }
    }

    return valid;
}
