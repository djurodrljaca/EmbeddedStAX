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

#include <EmbeddedStAX/Common/Attribute.h>
#include <EmbeddedStAX/XmlValidator/Name.h>
#include <EmbeddedStAX/XmlValidator/Attribute.h>

using namespace EmbeddedStAX::Common;

/**
 * Constructor
 *
 * \param name          Attribute name
 * \param value         Attribute value
 * \param quotationMark Attribute value's quotation mark
 *
 * \note Quotation mark is needed for the case that there is a unescaped quote character in the
 *       value string.
 */
Attribute::Attribute(const std::string &name,
                     const std::string &value,
                     const QuotationMark quotationMark)
    : m_name(name),
      m_value(value),
      m_quotationMark(quotationMark)
{
}

/**
 * Check if attribute is valid
 *
 * \retval true     Valid
 * \retval false    Invalid
 */
bool Attribute::isValid() const
{
    bool valid = XmlValidator::validateName(m_name);

    if (valid)
    {
        valid = XmlValidator::validateAttributeValue(m_value, m_quotationMark);
    }

    return valid;
}

/**
 * Clear
 */
void Attribute::clear()
{
    m_name.clear();
    m_value.clear();
}

/**
 * Get attribute name
 *
 * \return Attribute name
 */
std::string Attribute::name() const
{
    return m_name;
}

/**
 * Set attribute name
 *
 * \param name  Attribute name
 */
void Attribute::setName(const std::string &name)
{
    m_name = name;
}

/**
 * Get attribute value
 *
 * \return Attribute value
 */
std::string Attribute::value() const
{
    return m_value;
}

/**
 * Set attribute value
 *
 * \param value         Attribute value
 * \param quotationMark Attribute value's quotation mark
 *
 * \note Quotation mark is needed for the case that there is a unescaped quote character in the
 *       value string.
 */
void Attribute::setValue(const std::string &value, const QuotationMark quotationMark)
{
    m_value = value;
    m_quotationMark = quotationMark;
}

/**
 * Get attribute value's quotation mark
 *
 * \return Attribute value's quotation mark
 */
QuotationMark Attribute::valueQuotationMark() const
{
    return m_quotationMark;
}

void Attribute::setValueQuotationMark(const QuotationMark &quotationMark)
{
    m_quotationMark = quotationMark;
}
