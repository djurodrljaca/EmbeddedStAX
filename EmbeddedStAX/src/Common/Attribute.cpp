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
Attribute::Attribute(const UnicodeString &name,
                     const UnicodeString &value,
                     const QuotationMark quotationMark)
    : m_name(name),
      m_value(value),
      m_quotationMark(quotationMark)
{
}

/**
 * Copy constructor
 *
 * \param attribute Attribute
 */
Attribute::Attribute(const Attribute &other)
    : m_name(other.m_name),
      m_value(other.m_value),
      m_quotationMark(other.m_quotationMark)
{
}

Attribute &Attribute::operator=(const Attribute &other)
{
    if (this != &other)
    {
        m_name = other.m_name;
        m_value = other.m_value;
        m_quotationMark = other.m_quotationMark;
    }

    return *this;
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
UnicodeString Attribute::name() const
{
    return m_name;
}

/**
 * Set attribute name
 *
 * \param name  Attribute name
 */
void Attribute::setName(const UnicodeString &name)
{
    m_name = name;
}

/**
 * Get attribute value
 *
 * \return Attribute value
 */
UnicodeString Attribute::value() const
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
void Attribute::setValue(const UnicodeString &value, const QuotationMark quotationMark)
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

/**
 * Set attribute value's quotation mark
 *
 * \param quotationMark Attribute value's quotation mark
 */
void Attribute::setValueQuotationMark(const QuotationMark &quotationMark)
{
    m_quotationMark = quotationMark;
}

/**
 * Constructor
 */
AttributeList::AttributeList()
    : m_attributeList()
{
}

/**
 * Copy constructor
 *
 * \param other The input instance
 */
AttributeList::AttributeList(const AttributeList &other)
    : m_attributeList(other.m_attributeList)
{
}

/**
 * Destructor
 */
AttributeList::~AttributeList()
{
    clear();
}

/**
 * Assignment operator
 *
 * \param other The input instance
 *
 * \return Constant reference to this instance
 */
AttributeList &AttributeList::operator=(const AttributeList &other)
{
    if (&other != this)
    {
        m_attributeList = other.m_attributeList;
    }

    return *this;
}

/**
 * Clear the list
 */
void AttributeList::clear()
{
    m_attributeList.clear();
}

/**
 * Get size of the list
 */
size_t AttributeList::size() const
{
    return m_attributeList.size();
}

/**
 * Add attribute to the list
 *
 * \param attribute Attribute to add
 */
void AttributeList::add(const Attribute &attribute)
{
    m_attributeList.push_back(attribute);
}

/**
 * Search for attribute by name
 *
 * \param name  Name of the requested attribute
 *
 * \return Requested attribute or NULL if an attribute with the selected name was not found
 */
const Attribute *AttributeList::attribute(const UnicodeString &name)
{
    const Attribute *attribute = NULL;

    for (std::list<Attribute>::const_iterator it = m_attributeList.begin();
         it != m_attributeList.end();
         it++)
    {
        if (it->name() == name)
        {
            attribute = &(*it);
            break;
        }
    }

    return attribute;
}

/**
 * Get iterator that points to the first attribute in the list
 *
 * \return Attribute list iterator
 */
AttributeList::ConstIterator AttributeList::begin() const
{
    return m_attributeList.begin();
}

/**
 * Get iterator that points one past the last attribute in the list
 *
 * \return Attribute list iterator
 */
AttributeList::ConstIterator AttributeList::end() const
{
    return m_attributeList.end();
}
