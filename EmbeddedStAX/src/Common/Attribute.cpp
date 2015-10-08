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
AttributeList::Node::Node()
    : item(),
      next(NULL)
{
}

/**
 * Constructor
 *
 * \param attribute Attribute
 */
AttributeList::Node::Node(const Attribute &attribute)
    : item(attribute),
      next(NULL)
{
}

/**
 * Constructor
 *
 * \note Should only be used from the AttributeList class
 */
AttributeList::Iterator::Iterator(const Node *node)
    : m_node(node)
{
}

/**
 * Copy constructor
 *
 * \param other The input instance
 */
AttributeList::Iterator::Iterator(const Iterator &other)
    : m_node(other.m_node)
{
}

/**
 * Assignment operator
 *
 * \param other The input instance
 *
 * \return Constant reference to this instance
 */
AttributeList::Iterator &AttributeList::Iterator::operator=(const Iterator &other)
{
    if (this != &other)
    {
        this->m_node = other.m_node;
    }

    return *this;
}

/**
 * Get next attribute in the list
 */
void AttributeList::Iterator::next()
{
    if (m_node != NULL)
    {
        m_node = m_node->next;
    }
}

/**
 * Check if the current attribute in the iterator is valid
 *
 * \retval true     Attribute is valid
 * \retval false    Attribute is not valid
 */
bool AttributeList::Iterator::isValid()
{
    bool valid = false;

    if (m_node != NULL)
    {
        valid = true;
    }

    return valid;
}

/**
 * Get current attribute
 *
 * \return Current attribute or NULL pointer if iterator doesn't point to a valid list item
 */
const Attribute *AttributeList::Iterator::value() const
{
    const Attribute *attribute = NULL;

    if (m_node != NULL)
    {
        attribute = &(m_node->item);
    }

    return attribute;
}

/**
 * Constructor
 */
AttributeList::AttributeList()
    : m_size(0U),
      m_firstNode(NULL),
      m_lastNode(NULL)
{
}

/**
 * Copy constructor
 *
 * \param other The input instance
 */
AttributeList::AttributeList(const AttributeList &other)
    : m_size(0U),
      m_firstNode(NULL),
      m_lastNode(NULL)
{
    for (Iterator it = other.first(); it.isValid(); it.next())
    {
        const Attribute *attribute = it.value();

        if (attribute != NULL)
        {
            add(*attribute);
        }
        else
        {
            clear();
        }
    }
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
AttributeList &AttributeList::operator=(AttributeList other)
{
    // Input instance is created with a copy constructor, so we can just swap the member data
    const size_t tempSize = this->m_size;
    this->m_size = other.m_size;
    other.m_size = tempSize;

    Node * tempFirstNode = this->m_firstNode;
    this->m_firstNode = other.m_firstNode;
    other.m_firstNode = tempFirstNode;

    Node * tempLastNode = this->m_lastNode;
    this->m_lastNode = other.m_lastNode;
    other.m_lastNode = tempLastNode;

    return *this;
}

/**
 * Clear the list
 */
void AttributeList::clear()
{
    Node *node = m_firstNode;

    m_size = 0U;
    m_firstNode = NULL;
    m_lastNode = NULL;

    while (node != NULL)
    {
        Node *nextNode = node->next;
        delete node;
        node = nextNode;
    }
}

/**
 * Get size of the list
 */
size_t AttributeList::size() const
{
    return m_size;
}

/**
 * Add attribute to the list
 *
 * \param attribute Attribute to add
 */
void AttributeList::add(const Attribute &attribute)
{
    // Create new node
    Node *node = new Node(attribute);

    if (m_firstNode == NULL)
    {
        m_firstNode = node;
        m_lastNode = node;
        m_size = 1U;
    }
    else
    {
        m_lastNode->next = node;
        m_lastNode = node;
        m_size++;
    }
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
    Node *node = m_firstNode;

    while (node != NULL)
    {
        if (name == node->item.name())
        {
            attribute = &(node->item);
            break;
        }

        node = node->next;
    }

    return attribute;
}

/**
 * Search for attribute by index
 *
 * \param index Index of the requested attribute
 *
 * \return Requested attribute or NULL if an attribute with the selected index was not found
 */
const Attribute *AttributeList::attribute(const size_t index)
{
    const Attribute *attribute = NULL;

    if (index < m_size)
    {
        Node *node = m_firstNode;
        size_t currentIndex = 0U;

        while (node != NULL)
        {
            if (currentIndex == index)
            {
                attribute = &(node->item);
                break;
            }

            node = node->next;
            currentIndex++;
        }
    }

    return attribute;
}

/**
 * Get iterator that points to the first attribute in the list
 *
 * \return Attribute list iterator
 */
AttributeList::Iterator AttributeList::first() const
{
    return Iterator(m_firstNode);
}
