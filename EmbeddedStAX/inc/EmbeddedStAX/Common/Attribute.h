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

#ifndef EMBEDDEDSTAX_COMMON_ATTRIBUTE_H
#define EMBEDDEDSTAX_COMMON_ATTRIBUTE_H

#include <EmbeddedStAX/Common/Common.h>
#include <EmbeddedStAX/Common/Utf.h>

namespace EmbeddedStAX
{
namespace Common
{
class Attribute
{
public:
    // Public API
    Attribute(const UnicodeString &name = UnicodeString(),
              const UnicodeString &value = UnicodeString(),
              const QuotationMark quotationMark = QuotationMark_Quote);

    bool isValid() const;
    void clear();

    UnicodeString name() const;
    void setName(const UnicodeString &name);

    UnicodeString value() const;
    void setValue(const UnicodeString &value,
                  const QuotationMark quotationMark = QuotationMark_Quote);

    QuotationMark valueQuotationMark() const;
    void setValueQuotationMark(const QuotationMark &quotationMark);

private:
    // Private data
    UnicodeString m_name;
    UnicodeString m_value;
    QuotationMark m_quotationMark;
};
}
}

#endif // EMBEDDEDSTAX_COMMON_ATTRIBUTE_H
