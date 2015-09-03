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
#include "XmlWriter.h"

using namespace MiniSaxCpp;

/**
 * Constructor
 */
XmlWriter::XmlWriter()
    : m_message()
{
}

/**
 * Clear message
 */
void XmlWriter::clearMessage()
{
    m_message.clear();
}

/**
 * Get message string
 *
 * \return Message string
 */
std::string XmlWriter::getMessage() const
{
    return m_message;
}

/**
 * Add processing instruction to the message
 *
 * \param name          Processing instruction name
 * \param attributeList List of attributes
 *
 * \retval true     Success
 * \retval false    Error
 */
bool XmlWriter::addProcessingInstruction(const std::string &name,
        const AttributeList &attributeList)
{
    bool success = false;

    if (!name.empty())
    {
        std::string processingInstruction;

        // Add "start of processing" instruction string and the processing instruction name
        processingInstruction.append("<?").append(name);
        success = true;

        // Add attribute list to the message
        if (attributeList.size() > 0UL)
        {
            for (std::list<Attribute>::const_iterator iterator = attributeList.begin();
                    iterator != attributeList.end(); iterator++)
            {
                const std::string attributeString = XmlWriter::createAttributeString(*iterator);

                if (attributeString.empty())
                {
                    // Error: attribute name cannot be empty
                    success = false;
                }
                else
                {
                    // Add a space (separator) and the attribute in format: name="value"
                    processingInstruction.append(" ").append(attributeString);
                }

                // Exit loop on failure
                if (!success)
                {
                    break;
                }
            }
        }

        // Add "end of processing instruction" string
        if (success)
        {
            processingInstruction.append("?>");
        }

        // Add processing instruction string to the message
        if (success)
        {
            m_message.append(processingInstruction);
        }
    }

    return success;
}

/**
 * Add a self-closing element to the message
 *
 * \param elementName   Processing instruction name
 * \param attributeList List of attributes
 *
 * \retval true     Success
 * \retval false    Error
 */
bool XmlWriter::addSelfClosingElement(const std::string &elementName, const AttributeList &attributeList)
{
    bool success = false;

    if (!elementName.empty())
    {
        std::string elementString;

        // Add "start of tag" character and the element name
        elementString.append("<").append(elementName);
        success = true;

        // Add attribute list to the message
        if (attributeList.size() > 0UL)
        {
            for (std::list<Attribute>::const_iterator iterator = attributeList.begin();
                    iterator != attributeList.end(); iterator++)
            {
                const std::string attributeString = XmlWriter::createAttributeString(*iterator);

                if (attributeString.empty())
                {
                    // Error: attribute name cannot be empty
                    success = false;
                }
                else
                {
                    // Add a space (separator) and the attribute in format: name="value"
                    elementString.append(" ").append(attributeString);
                }

                // Exit loop on failure
                if (!success)
                {
                    break;
                }
            }
        }

        // Add "end of element" string
        if (success)
        {
            elementString.append("/>");
        }

        // Add element string to the message
        if (success)
        {
            m_message.append(elementString);
        }
    }

    return success;
}

/**
 * Add start of element to the message
 *
 * \param elementName   Processing instruction name
 * \param attributeList List of attributes
 *
 * \retval true     Success
 * \retval false    Error
 */
bool XmlWriter::startElement(const std::string &elementName, const AttributeList &attributeList)
{
    bool success = false;

    if (!elementName.empty())
    {
        std::string elementString;

        // Add "start of tag" character and the element name
        elementString.append("<").append(elementName);
        success = true;

        // Add attribute list to the message
        if (attributeList.size() > 0UL)
        {
            for (std::list<Attribute>::const_iterator iterator = attributeList.begin();
                    iterator != attributeList.end(); iterator++)
            {
                const std::string attributeString = XmlWriter::createAttributeString(*iterator);

                if (attributeString.empty())
                {
                    // Error: attribute name cannot be empty
                    success = false;
                }
                else
                {
                    // Add a space (separator) and the attribute in format: name="value"
                    elementString.append(" ").append(attributeString);
                }

                // Exit loop on failure
                if (!success)
                {
                    break;
                }
            }
        }

        // Add "end of tag" character
        if (success)
        {
            elementString.append(">");
        }

        // Add element string to the message
        if (success)
        {
            m_message.append(elementString);
        }
    }

    return success;
}

bool XmlWriter::endElement(const std::string &elementName)
{
    bool success = false;

    if (!elementName.empty())
    {
        std::string elementString;

        // Add "start of closing tag" string, the element name and "end of tag" character
        elementString.append("</").append(elementName).append(">");
        
        // Add element string to the message
        m_message.append(elementString);
        success = true;

    }
    
    return success;
}

void XmlWriter::addTextNode(const std::string &textNode)
{
    m_message.append(textNode);
}

void XmlWriter::addComment(const std::string &text)
{
    // Add comment to the message
    m_message.append("<!--").append(text).append("-->");
}

std::string XmlWriter::createAttributeString(const Attribute &attribute)
{
    std::string attributeString;

    if (!attribute.name.empty())
    {
        // Create attribute string in format: name="value"
        attributeString.append(attribute.name).append("=\"").append(attribute.value).append("\"");
    }

    return attributeString;
}
