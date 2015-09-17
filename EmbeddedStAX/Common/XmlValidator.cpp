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

using namespace EmbeddedStAX;

static bool parseDigit(const char digitCharacter, const uint32_t base, uint32_t *digitValue);
static bool validateNumberInCharRefString(const std::string &str,
                                          const size_t startPosition,
                                          const uint32_t base,
                                          size_t *nextCharacterPosition = NULL);
static bool validateCharRefString(const std::string &str,
                                  const size_t startPosition,
                                  size_t *nextCharacterPosition = NULL);
static bool validateEntityRefString(const std::string &str,
                                    const size_t startPosition,
                                    size_t *nextCharacterPosition = NULL);
static bool validateReferenceString(const std::string &str,
                                    const size_t startPosition,
                                    size_t *nextCharacterPosition = NULL);

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
static bool parseDigit(const char digitCharacter, const uint32_t base, uint32_t *digitValue)
{
    bool success = false;

    if (digitValue != NULL)
    {
        switch (base)
        {
            case 10U:
            {
                if (('0' <= digitCharacter) && (digitCharacter <= '9'))
                {
                    *digitValue = (uint32_t)(digitCharacter - '0');
                    success = true;
                }

                break;
            }

            case 16U:
            {
                if (('0' <= digitCharacter) && (digitCharacter <= '9'))
                {
                    *digitValue = (uint32_t)(digitCharacter - '0');
                    success = true;
                }
                else if (('a' <= digitCharacter) && (digitCharacter <= 'f'))
                {
                    *digitValue = (uint32_t)(digitCharacter - 'a');
                    success = true;
                }
                else if (('A' <= digitCharacter) && (digitCharacter <= 'F'))
                {
                    *digitValue = (uint32_t)(digitCharacter - 'A');
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

/**
 * This is a helper function for validateCharRefString function.
 *
 * \param      str                      UTF-8 encoded string
 * \param      startPosition            Position of the first digit in str (after "&#" or "&#x"
 *                                      sequence)
 * \param      base                     Digit's base (10 for decimal and 16 for hexadecimal)
 * \param[out] nextCharacterPosition    Optional output for next character after the valid CharRef
 *                                      string
 *
 * \retval true     Success
 * \retval false    Error
 */
static bool validateNumberInCharRefString(const std::string &str,
                                          const size_t startPosition,
                                          const uint32_t base,
                                          size_t *nextCharacterPosition)
{
    bool valid = false;

    enum State
    {
        State_Started,
        State_Parsing,
        State_Success,
        State_Error
    };

    uint32_t unicodeCharacter = 0U;
    State state = State_Started;
    size_t position = startPosition;

    while ((position < str.size()) &&
           (state != State_Success) &&
           (state != State_Error))
    {
        const char character = str.at(position);

        switch (state)
        {
            case State_Started:
            {
                // Get first digit
                uint32_t value = 0U;

                if (parseDigit(character, base, &value))
                {
                    unicodeCharacter = value;
                    state = State_Parsing;
                }
                else
                {
                    // Error invalid digit value
                    state = State_Error;
                }

                break;
            }

            case State_Parsing:
            {
                // Check for end of CharRef
                if (character == ';')
                {
                    // Check if unicodeCharacter is valid
                    if (Utf::isUnicodeCharacterValid(unicodeCharacter))
                    {
                        // Successfully validate the CharRef
                        if (nextCharacterPosition != NULL)
                        {
                            *nextCharacterPosition = position + 1U;
                        }

                        state = State_Success;
                        valid = true;
                    }
                }
                else
                {
                    // Get next digit
                    uint32_t value = 0U;

                    if (parseDigit(character, base, &value))
                    {
                        unicodeCharacter = (unicodeCharacter * base) + value;

                        // Check if unicode character is valid
                        const bool unicodeCharacterValid =
                                Utf::isUnicodeCharacterValid(unicodeCharacter);

                        if (!unicodeCharacterValid)
                        {
                            // Error invalid unicode character value
                            state = State_Error;
                        }
                    }
                    else
                    {
                        // Error invalid digit value
                        state = State_Error;
                    }
                }

                break;
            }

            default:
            {
                // Error, invalid state
                state = State_Error;
                break;
            }
        }

        // Check next character
        position++;
    }

    return valid;
}

/**
 * Check if str contains a CharRef at the specified position
 *
 * \param      str                      UTF-8 encoded string
 * \param      startPosition            Position of the '&' character in str where CharRef should be
 *                                      located
 * \param[out] nextCharacterPosition    Optional output for next character after the valid CharRef
 *                                      string
 *
 * Valid AttValue string format:
 * \code{.unparsed}
 * CharRef in decimal format     ::= '&#' [0-9]+ ';'
 * CharRef in hexadecimal format ::= '&#x' [0-9a-fA-F]+ ';'
 * \endcode
 */
static bool validateCharRefString(const std::string &str,
                                  const size_t startPosition,
                                  size_t *nextCharacterPosition)
{
    bool valid = false;

    // Check for CharRef in hexadecimal format
    if (str.substr(startPosition, 3U) == std::string("&#x"))
    {
        // Validate CharRef in hexadecimal format
        valid = validateNumberInCharRefString(str, startPosition + 3U, 16U, nextCharacterPosition);
    }
    // Check for CharRef in decimal format
    else if (str.substr(startPosition, 2U) == std::string("&#"))
    {
        // Validate CharRef in decimal format
        valid = validateNumberInCharRefString(str, startPosition + 2U, 10U, nextCharacterPosition);
    }
    else
    {
        // Error, invalid CharRef string
    }

    return valid;
}

/**
 * Check if str contains a EntityRef at the specified position
 *
 * \param      str                      UTF-8 encoded string
 * \param      startPosition            Position of the '&' character in str where EntityRef should
 *                                      be located
 * \param[out] nextCharacterPosition    Optional output for next character after the valid EntityRef
 *                                      string
 *
 * Valid AttValue string format:
 * \code{.unparsed}
 * EntityRef ::= '&' Name ';'
 * \endcode
 */
static bool validateEntityRefString(const std::string &str,
                                    const size_t startPosition,
                                    size_t *nextCharacterPosition)
{
    bool valid = false;

    if (str.size() > (startPosition + 2U))
    {
        if (str.at(startPosition) == '&')
        {
            // Validate start character
            uint32_t unicodeCharacter = 0U;
            size_t currentPosition = startPosition + 1;
            size_t nextPosition = 0U;

            Utf::Result result = Utf::unicodeCharacterFromUtf8(str,
                                                               currentPosition,
                                                               &nextPosition,
                                                               &unicodeCharacter);

            if (result == Utf::Result_Success)
            {
                if (XmlValidator::isNameStartChar(unicodeCharacter))
                {
                    // NameStartChar is valid
                    valid = true;
                }
            }

            // Validate the rest of the characters
            while (valid && (nextPosition < str.size()))
            {
                // Validate next character
                currentPosition = nextPosition;
                result = Utf::unicodeCharacterFromUtf8(str,
                                                       currentPosition,
                                                       &nextPosition,
                                                       &unicodeCharacter);

                valid = false;

                if (result == Utf::Result_Success)
                {
                    // Check for end of EntityRef
                    if (unicodeCharacter == (uint32_t)';')
                    {
                        // End of EntityRef found, finish validation
                        if (nextCharacterPosition != NULL)
                        {
                            *nextCharacterPosition = nextPosition;
                        }

                        valid = true;
                        break;
                    }
                    // Check for NameChar
                    else if (XmlValidator::isNameChar(unicodeCharacter))
                    {
                        // NameChar is valid
                        valid = true;
                    }
                }
            }
        }
    }

    return valid;
}

/**
 * Check if str contains a Reference at the specified position
 *
 * \param      str                      UTF-8 encoded string
 * \param      startPosition            Position of the '&' character in str where Reference should
 *                                      be located
 * \param[out] nextCharacterPosition    Optional output for next character after the valid EntityRef
 *                                      string
 *
 * Valid AttValue string format:
 * \code{.unparsed}
 * Reference ::= EntityRef | CharRef
 * \endcode
 */
static bool validateReferenceString(const std::string &str,
                                    const size_t startPosition,
                                    size_t *nextCharacterPosition)
{
    bool valid = false;

    // Check for Reference (EntityRef or CharRef)
    if (str.size() > (startPosition + 2U))
    {
        // Check for CharRef (starts with "&#" sequence)
        if (str.at(startPosition + 1U) == '#')
        {
            valid = validateCharRefString(str, startPosition);
        }
        // Check for EntityRef (starts with '&' character)
        else
        {
            valid = validateEntityRefString(str, startPosition);
        }
    }

    return valid;
}

/**
 * This function can be used to validate a whitespace character
 *
 * \param whitespaceChar    Unicode character
 *
 * \retval true     Whitespace character is valid
 * \retval false    Whitespace character is not valid
 *
 * Allowed values for NameStartChar:
 *  - 0x09
 *  - 0x0A
 *  - 0x0D
 *  - 0x20
 */
bool XmlValidator::isWhitespace(const uint32_t whitespaceChar)
{
    bool valid = false;

    if (whitespaceChar == 0x09U)
    {
        valid = true;
    }
    else if (whitespaceChar == 0x0AU)
    {
        valid = true;
    }
    else if (whitespaceChar == 0x0DU)
    {
        valid = true;
    }
    else if (whitespaceChar == 0x20U)
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
 * This function can be used to validate a "NameStartChar"
 *
 * \param nameStartChar Unicode character
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
bool XmlValidator::isNameStartChar(const uint32_t nameStartChar)
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
 *  - NameStartChar
 *  - '-'
 *  - '.'
 *  - [0 - 9]
 *  - 0xB7
 *  - [0x0300 - 0x036F]
 *  - [0x203F - 0x2040]
 */
bool XmlValidator::isNameChar(const uint32_t nameChar)
{
    bool valid = isNameStartChar(nameChar);

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

        Utf::Result result = Utf::unicodeCharacterFromUtf8(name,
                                                           currentPosition,
                                                           &nextPosition,
                                                           &unicodeCharacter);

        if (result == Utf::Result_Success)
        {
            if (isNameStartChar(unicodeCharacter))
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
            result = Utf::unicodeCharacterFromUtf8(name,
                                                   currentPosition,
                                                   &nextPosition,
                                                   &unicodeCharacter);

            valid = false;

            if (result == Utf::Result_Success)
            {
                if (isNameChar(unicodeCharacter))
                {
                    // NameChar is valid
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
 * \param commentText   UTF-8 encoded Comment string
 *
 * \retval true     Valid
 * \retval false    Invalid
 *
 * Valid Comment string format:
 * \code{.unparsed}
 * Comment ::= ((Char - '-') | ('-' (Char - '-')))*
 * \endcode
 */
bool XmlValidator::validateCommentString(const std::string &commentText)
{
    bool valid = false;

    if (commentText.empty())
    {
        // Comment can be empty
        valid = true;
    }
    else
    {
        // Comment must not be a single '-' character
        if (commentText != "-")
        {
            valid = true;
        }

        // Comment must not contain "--" sequence or end with a '-' character, otherwise all Char
        // characters are allowed
        if (valid)
        {
            uint32_t unicodeCharacter = 0U;
            size_t currentPosition = 0U;
            size_t nextPosition = 0U;

            do
            {
                // Validate character
                Utf::Result result = Utf::unicodeCharacterFromUtf8(commentText,
                                                                   currentPosition,
                                                                   &nextPosition,
                                                                   &unicodeCharacter);

                valid = false;

                if (result == Utf::Result_Success)
                {
                    // Check if Char character
                    if (isChar(unicodeCharacter))
                    {
                        // Character is valid
                        valid = true;
                    }
                }

                if (valid)
                {
                    if (unicodeCharacter == (uint32_t)'-')
                    {
                        // Check if comment ends with '-' character
                        if (nextPosition >= commentText.size())
                        {
                            // Error, ends with '-' character
                            valid = false;
                        }
                        else
                        {
                            // Check for "--" sequence
                            if (commentText.at(nextPosition) == '-')
                            {
                                // Error, "--" sequence detected
                                valid = false;
                            }
                        }
                    }
                }

                if (valid)
                {
                    // Prepare for next loop cycle
                    currentPosition = nextPosition;
                }
            }
            while (valid && (nextPosition < commentText.size()));
        }
    }

    return valid;
}

/**
 * This function can be used to validate a PITarget string
 *
 * \param piTarget  UTF-8 encoded PITarget string
 *
 * \retval true     Valid
 * \retval false    Invalid
 *
 * Valid Name string format:
 * \code{.unparsed}
 * PITarget ::= Name - (('X' | 'x') ('M' | 'm') ('L' | 'l'))
 * \endcode
 */
bool XmlValidator::validatePiTargetString(const std::string &piTarget)
{
    bool valid = false;

    if (!piTarget.empty())
    {
        // PITarget must not equal "xml" (case-insensitive)
        if (piTarget.size() == 3U)
        {
            if ((piTarget.at(0U) != 'x') && (piTarget.at(0U) != 'X') &&
                (piTarget.at(1U) != 'm') && (piTarget.at(1U) != 'M') &&
                (piTarget.at(2U) != 'l') && (piTarget.at(2U) != 'L'))
            {
                valid = true;
            }
        }
        else
        {
            valid = true;
        }
    }

    return valid;
}

/**
 * This function can be used to validate a PI's value string
 *
 * \param piValue   UTF-8 encoded PI's value string
 *
 * \retval true     Valid
 * \retval false    Invalid
 *
 * Valid Name string format:
 * \code{.unparsed}
 * value ::= (Char* - (Char* '?>' Char*)))?
 * \endcode
 */
bool XmlValidator::validatePiValueString(const std::string &piValue)
{
    bool valid = false;

    if (piValue.empty())
    {
        // PI's value can be empty
        valid = true;
    }
    else
    {
        // PI's value must not contain "?>" sequence otherwise any Char character is allowed
        uint32_t lastUnicodeCharacter = 0U;
        uint32_t unicodeCharacter = 0U;
        size_t currentPosition = 0U;
        size_t nextPosition = 0U;

        do
        {
            // Validate character
            Utf::Result result = Utf::unicodeCharacterFromUtf8(piValue,
                                                               currentPosition,
                                                               &nextPosition,
                                                               &unicodeCharacter);

            valid = false;

            if (result == Utf::Result_Success)
            {
                // Check if Char character
                if (isChar(unicodeCharacter))
                {
                    // Character is valid
                    valid = true;
                }
            }

            if (valid)
            {
                // Check for "?>" sequence
                if ((lastUnicodeCharacter == (uint32_t)'?') &&
                    (unicodeCharacter == (uint32_t)'>'))
                {
                    // Error, "?>" sequence detected
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
        while (valid && (nextPosition < piValue.size()));
    }

    return valid;
}

/**
 * This function can be used to validate a AttValue string
 *
 * \param attValue      UTF-8 encoded AttValue string
 * \param quotationMark Quotation mark
 *
 * \retval true     Valid
 * \retval false    Invalid
 *
 * Valid AttValue string format:
 * \code{.unparsed}
 * AttValue with quote quotation mark      ::= ([^<&"] | Reference)*
 * AttValue with apostrophe quotation mark ::= ([^<&'] | Reference)*
 *
 * Reference ::= EntityRef | CharRef
 * \endcode
 *
 * \note This function only checks the actual value (without the outer quotes)
 */
bool XmlValidator::validateAttValueString(const std::string &attValue,
                                          const Common::QuotationMark quotationMark)
{
    bool valid = false;

    if (attValue.empty())
    {
        // AttValue can be empty
        valid = true;
    }
    else
    {
        // AttValue must not contain characters: '<', '&' and ('"' or '\''), with the exception of
        // "&" character when it is the start of a valid Reference sequence
        uint32_t unicodeCharacter = 0U;
        size_t currentPosition = 0U;
        size_t nextPosition = 0U;

        do
        {
            // Validate character
            Utf::Result result = Utf::unicodeCharacterFromUtf8(attValue,
                                                               currentPosition,
                                                               &nextPosition,
                                                               &unicodeCharacter);

            valid = false;

            if (result == Utf::Result_Success)
            {
                switch (unicodeCharacter)
                {
                    case (uint32_t)'<':
                    {
                        // Error, invalid character
                        break;
                    }

                    case (uint32_t)'&':
                    {
                        // Check for Reference
                        valid = validateReferenceString(attValue, currentPosition, &nextPosition);
                        break;
                    }

                    case (uint32_t)'"':
                    {
                        if (quotationMark == Common::QuotationMark_Apostrophe)
                        {
                            valid = true;
                        }

                        break;
                    }

                    case (uint32_t)'\'':
                    {
                        if (quotationMark == Common::QuotationMark_Quote)
                        {
                            valid = true;
                        }

                        break;
                    }

                    default:
                    {
                        valid = true;
                        break;
                    }
                }
            }

            if (valid)
            {
                // Prepare for next loop cycle
                currentPosition = nextPosition;
            }
        }
        while (valid && (nextPosition < attValue.size()));
    }

    return valid;
}

/**
 * This function can be used to validate a TextNode string
 *
 * \param textNode  UTF-8 encoded TextNode string
 *
 * \retval true     Valid
 * \retval false    Invalid
 *
 * Valid Name string format:
 * \code{.unparsed}
 * TextNode ::= CharData? (Reference CharData?)*
 *
 * CharData ::= [^<&]* - ([^<&]* ']]>' [^<&]*)
 * \endcode
 *
 * \note The "TextNode" is a custom entity and it combines CharData and References
 */
bool XmlValidator::validateTextNodeString(const std::string &textNode)
{
    bool valid = false;

    if (textNode.empty())
    {
        // TextNode can be empty (does not make sense to have an empty text node, but it is valid)
        valid = true;
    }
    else
    {
        // TextNode must not contain '<' and '&' characters or "]]>" sequence, otherwise any valid
        // unicode character is allowed. An exception is that '&' character is allowed if it is the
        // start of a valid Reference sequence
        uint32_t unicodeCharacter = 0U;
        size_t currentPosition = 0U;
        size_t nextPosition = 0U;

        do
        {
            // Validate character
            Utf::Result result = Utf::unicodeCharacterFromUtf8(textNode,
                                                               currentPosition,
                                                               &nextPosition,
                                                               &unicodeCharacter);

            valid = false;

            if (result == Utf::Result_Success)
            {
                switch (unicodeCharacter)
                {
                    case (uint32_t)'<':
                    {
                        // Error, invalid character
                        break;
                    }

                    case (uint32_t)'&':
                    {
                        // Check for Reference
                        valid = validateReferenceString(textNode, currentPosition, &nextPosition);
                        break;
                    }

                    case (uint32_t)'>':
                    {
                        // The "]]>" sequence is not allowed
                        if (currentPosition < 2U)
                        {
                            // Not enough characters for the complete "]]>" sequence
                            valid = true;
                        }
                        else
                        {
                            if (textNode.substr(currentPosition - 2U, 3U) != std::string("]]>"))
                            {
                                // The '>' character is not part of a "]]>" sequence
                                valid = true;
                            }
                        }
                        break;
                    }

                    default:
                    {
                        valid = true;
                        break;
                    }
                }
            }

            if (valid)
            {
                // Prepare for next loop cycle
                currentPosition = nextPosition;
            }
        }
        while (valid && (nextPosition < textNode.size()));
    }

    return valid;
}

/**
 * This function can be used to validate a CData string
 *
 * \param cData UTF-8 encoded CData string
 *
 * \retval true     Valid
 * \retval false    Invalid
 *
 * Valid Name string format:
 * \code{.unparsed}
 * CData ::= Char* - (Char* ']]>' Char*)
 * \endcode
 */
bool XmlValidator::validateCDataString(const std::string &cData)
{
    bool valid = false;

    if (cData.empty())
    {
        // CData can be empty (does not make sense to have an empty CData, but it is valid)
        valid = true;
    }
    else
    {
        // CData can contain only Char characters as long as they are not part of a "]]>" sequence
        uint32_t unicodeCharacter = 0U;
        size_t currentPosition = 0U;
        size_t nextPosition = 0U;

        do
        {
            // Validate character
            Utf::Result result = Utf::unicodeCharacterFromUtf8(cData,
                                                               currentPosition,
                                                               &nextPosition,
                                                               &unicodeCharacter);

            valid = false;

            if (result == Utf::Result_Success)
            {
                // Check for Char
                valid = isChar(unicodeCharacter);

                if (valid)
                {
                    // Check for "]]>" sequence
                    if (unicodeCharacter == (uint32_t)'>')
                    {
                        if (currentPosition >= 2U)
                        {
                            if (cData.substr(currentPosition - 2U, 3U) == std::string("]]>"))
                            {
                                // Error, the "]]>" sequence is not allowed
                                valid = false;
                            }
                        }
                    }
                }
            }

            if (valid)
            {
                // Prepare for next loop cycle
                currentPosition = nextPosition;
            }
        }
        while (valid && (nextPosition < cData.size()));
    }

    return valid;
}
