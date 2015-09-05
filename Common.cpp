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
#include <cinttypes>
#include <cstdio>

using namespace MiniSaxCpp;

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
std::string Common::escapeSpecialCharacter(const char specialCharacter)
{
    std::string name;
    
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
std::string Common::escapeCharacterDecimal(const uint32_t unicodeCharacter)
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
        
        const int32_t returnValue = std::snprintf(str, sizeof(str), formatString, unicodeCharacter);
        
        // Check if expected number of characters were written to str
        if ((3 < returnValue) && (returnValue < sizeof(str)))
        {
            escapedString = std::string(str);
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
std::string Common::escapeCharacterHexadecimal(const uint32_t unicodeCharacter)
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
        
        const int32_t returnValue = std::snprintf(str, sizeof(str), formatString, unicodeCharacter);
        
        // Check if expected number of characters were written to str
        if ((3 < returnValue) && (returnValue < sizeof(str)))
        {
            escapedString = std::string(str);
        }
    }
    
    return escapedString;
}