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

#include <EmbeddedStAX/Common/DocumentType.h>
#include <EmbeddedStAX/XmlValidator/Name.h>

using namespace EmbeddedStAX::Common;

/**
 * Constructor
 *
 * \param name  Name of the root element
 */
DocumentType::DocumentType(const std::string &name)
    : m_name(name)
{
}

/**
 * Check if processing instruction is valid
 *
 * \retval true     Valid
 * \retval false    Invalid
 */
bool DocumentType::isValid() const
{
    return XmlValidator::validateName(m_name);
}

/**
 * Clear
 */
void DocumentType::clear()
{
    m_name.clear();
}

/**
 * Get name of the root element
 *
 * \return Name of the root element
 */
std::string DocumentType::name() const
{
    return m_name;
}

/**
 * Set processing instruction name
 *
 * \param piTarget  Processing instruction name
 */
void DocumentType::setName(const std::string &name)
{
    m_name = name;
}
