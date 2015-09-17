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

#ifndef MINISAXCPP_UTF_H
#define MINISAXCPP_UTF_H

#include <string>
#include <stdint.h>

namespace EmbeddedStAX
{
typedef std::basic_string<uint32_t> UnicodeString;

class Utf8
{
public:
    // Public types
    enum Result
    {
        Result_Success,
        Result_Incomplete,
        Result_Error
    };

public:
    // Public API
    Utf8();

    void clear();
    Result write(const char data);

    uint32_t getUnicodeCharacter() const;

    static std::string toUtf8(const uint32_t unicodeCharacter);
    static std::string toUtf8(const UnicodeString &unicodeString);

private:
    // Private API
    Result writeFirstCharacter(const char data);
    Result writeNextCharacter(const char data);

private:
    // Private data
    std::string m_buffer;
    size_t m_noOfBytes;
    uint32_t m_value;
    std::string m_utf8;
};

// TODO: merge it with Utf8 ?
namespace Utf
{
enum Result
{
    Result_Success,
    Result_Incomplete,
    Result_Error
};

Result unicodeCharacterFromUtf8(const std::string &utf8,
                                const size_t startPosition,
                                size_t *nextCharacterPosition,
                                uint32_t *unicodeCharacter);

bool isUnicodeCharacterValid(const uint32_t unicodeCharacter);
}
}

#endif // MINISAXCPP_UTF_H
