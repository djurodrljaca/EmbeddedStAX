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

#ifndef MINISAXCPP_COMMON_H
#define MINISAXCPP_COMMON_H

#include <string>
#include <stdint.h>

namespace MiniSaxCpp
{
namespace Common
{
enum XmlVersion
{
    XmlVersion_None,
    XmlVersion_Unknown,
    XmlVersion_v1_0
};

enum XmlEncoding
{
    XmlEncoding_None,
    XmlEncoding_Unknown,
    XmlEncoding_Utf8
};

enum XmlStandalone
{
    XmlStandalone_None,
    XmlStandalone_Unknown,
    XmlStandalone_No,
    XmlStandalone_Yes
};

enum QuotationMark
{
    QuotationMark_Quote,
    QuotationMark_Apostrophe
};

std::string escapeAttributeValueString(const std::string &attributeValue,
                                       const Common::QuotationMark quotationMark);
std::string escapeTextNodeString(const std::string &textNode);

class DataBuffer
{
public:
    // Public API
    DataBuffer(const size_t size);
    ~DataBuffer();

    void clear();
    bool empty() const;
    bool full() const;
    size_t free() const;
    size_t used() const;

    bool write(const char data);
    char read(bool *success = NULL);

private:
    // Private data
    char *m_dataBuffer;
    size_t m_dataBufferSize;
    size_t m_writePosition;
    size_t m_readPosition;
};
}
}

#endif // MINISAXCPP_COMMON_H
