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

#ifndef EMBEDDEDSTAX_COMMON_XMLDECLARATION_H
#define EMBEDDEDSTAX_COMMON_XMLDECLARATION_H

#include <EmbeddedStAX/Common/Utf.h>

namespace EmbeddedStAX
{
namespace Common
{
class XmlDeclaration
{
public:
    // Public types
    enum Version
    {
        Version_None,
        Version_Invalid,
        Version_v1_0
    };

    enum Encoding
    {
        Encoding_None,
        Encoding_Invalid,
        Encoding_Utf8
    };

    enum Standalone
    {
        Standalone_None,
        Standalone_Invalid,
        Standalone_No,
        Standalone_Yes
    };

public:
    // Public API
    XmlDeclaration(const Version version = Version_None,
                   const Encoding encoding = Encoding_None,
                   const Standalone standalone = Standalone_None);
    XmlDeclaration(const XmlDeclaration &other);

    XmlDeclaration &operator=(const XmlDeclaration &other);

    bool isValid() const;
    void clear();

    Version version() const;
    void setVersion(const Version version);

    Encoding encoding() const;
    void setEncoding(const Encoding encoding);

    Standalone standalone() const;
    void setStandalone(const Standalone standalone);

    static XmlDeclaration fromPiData(const UnicodeString &piData);

private:
    // Private API
    static size_t skipWhitespace(const size_t startPosition, const UnicodeString &data);
    static UnicodeString extractValue(const size_t startPosition,
                                      const UnicodeString &data,
                                      size_t *nextPosition);

private:
    // Private data
    Version m_version;
    Encoding m_encoding;
    Standalone m_standalone;
};
}
}

#endif // EMBEDDEDSTAX_COMMON_XMLDECLARATION_H
