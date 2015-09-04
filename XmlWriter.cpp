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
#include "Utf.h"
#include "XmlValidator.h"

using namespace MiniSaxCpp;

/**
 * Constructor
 */
XmlWriter::XmlWriter()
{
    clearMessage();
}

/**
 * Clear message
 */
void XmlWriter::clearMessage()
{
    m_state = State_Empty;
    m_xmlDeclarationSet = false;
    m_elementNameList.clear();
    m_message.clear();
}

/**
 * Get message string
 *
 * \return Message string
 * \return Empty string on error
 *
 * \note Message string will only be returned if XML document has been fully completed (root element
 *       has to be ended), otherwise an empty string will be returned.
 */
std::string XmlWriter::getMessage() const
{
    std::string message;

    if (m_state == State_DocumentEnded)
    {
        message = m_message;
    }

    return message;
}

/**
 * Sets XML Declaration to the message.
 *
 * \param name          Processing instruction name
 * \param attributeList List of attributes
 *
 * \retval true     Success
 * \retval false    Error, message is not empty
 *
 * \note Currently only XML Declaration with parameters version 1.0 with encoding "UTF-8" are
 *       supported!
 */
bool XmlWriter::setXmlDeclaration()
{
    bool success = false;

    if (m_state == State_Empty)
    {
        // Set XML Declaration string
        m_message = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
        
        m_xmlDeclarationSet = true;
        m_state = State_DocumentStarted;
        success = true;
    }

    return success;
}

bool XmlWriter::addComment(const std::string &comment)
{
    bool success = false;
    State nextState = m_state;
    std::string prependString;

    switch (m_state)
    {
        case State_Empty:
        {
            nextState = State_DocumentStarted;
            success = true;
            break;
        }
        
        case State_ElementStarted:
        {
            // Close start tag of current element
            prependString = ">";
            nextState = State_InElement;
            success = true;
            break;
        }
        
        case State_DocumentStarted:
        case State_InElement:
        case State_DocumentEnded:
        {
            success = true;
            break;
        }
        
        default:
        {
            break;
        }
    }
    
    if (success)
    {
        if (XmlValidator::validateCommentString(comment))
        {
            if (!prependString.empty())
            {
                m_message.append(prependString);
            }
            
            m_message.append("<!--").append(comment).append("-->");
            m_state = nextState;
            success = true;
        }
    }

    return success;
}

bool XmlWriter::startElement(const std::string &elementName)
{
    bool success = false;
    std::string prependString;

    if (XmlValidator::validateNameString(elementName))
    {
        switch (m_state)
        {
            case State_Empty:
            case State_DocumentEnded:
            case State_InElement:
            {
                success = true;
                break;
            }
            
            case State_ElementStarted:
            {
                // Close start tag of current element
                prependString = ">";
                success = true;
                break;
            }

            default:
            {
                break;
            }
        }
    }

    if (success)
    {
        if (!prependString.empty())
        {
            m_message.append(prependString);
        }
        
        m_message.append("<").append(elementName);
        m_elementNameList.push_back(elementName);
        m_state = State_ElementStarted;
        success = true;
    }

    return success;
}

bool XmlWriter::addAttribute(const Attribute &attribute)
{
    bool success = false;
    
    // TODO: implement

    return success;
}










//-----------------------------------------------


















// /**
//  * Add a self-closing element to the message
//  *
//  * \param elementName   Processing instruction name
//  * \param attributeList List of attributes
//  *
//  * \retval true     Success
//  * \retval false    Error
//  */
// bool XmlWriter::addSelfClosingElement(const std::string &elementName,
//                                       const AttributeList &attributeList)
// {
//     bool success = false;
//     std::string elementString = createStartOfElementString(elementName, attributeList);
// 
//     if (!elementString.empty())
//     {
//         // Add "end of element" string
//         elementString.append("/>");
// 
//         // Add element string to the message
//         m_message.append(elementString);
//         success = true;
//     }
// 
//     return success;
// }
// 
// /**
//  * Add start of element to the message
//  *
//  * \param elementName   Processing instruction name
//  * \param attributeList List of attributes
//  *
//  * \retval true     Success
//  * \retval false    Error
//  */
// bool XmlWriter::startElement(const std::string &elementName, const AttributeList &attributeList)
// {
//     bool success = false;
//     std::string elementString = createStartOfElementString(elementName, attributeList);
// 
//     if (!elementString.empty())
//     {
//         // Add "end of start tag" character
//         elementString.append(">");
// 
//         // Add element string to the message
//         m_message.append(elementString);
//         success = true;
//     }
// 
//     return success;
// }
// 
// bool XmlWriter::endElement(const std::string &elementName)
// {
//     bool success = false;
// 
//     if (!elementName.empty())
//     {
//         std::string elementString;
// 
//         // Add "start of closing tag" string, the element name and "end of tag" character
//         elementString.append("</").append(elementName).append(">");
// 
//         // Add element string to the message
//         m_message.append(elementString);
//         success = true;
// 
//     }
// 
//     return success;
// }
// 
// void XmlWriter::addTextNode(const std::string &textNode)
// {
//     m_message.append(textNode);
// }
// 
// void XmlWriter::addComment(const std::string &text)
// {
//     // Add comment to the message
//     m_message.append("<!--").append(text).append("-->");
// }
// 
// std::string XmlWriter::createAttributeString(const Attribute &attribute)
// {
//     std::string attributeString;
// 
//     if (!attribute.name.empty())
//     {
//         // Create attribute string in format: name="value"
//         attributeString.append(attribute.name).append("=\"").append(attribute.value).append("\"");
//     }
// 
//     return attributeString;
// }
// 
// std::string XmlWriter::createStartOfElementString(const std::string &elementName,
//         const AttributeList &attributeList)
// {
//     std::string elementString;
// 
//     if (!elementName.empty())
//     {
//         // Add "start of tag" character and the element name
//         elementString.append("<").append(elementName);
// 
//         // Add attribute list to the message
//         if (attributeList.size() > 0UL)
//         {
//             for (std::list<Attribute>::const_iterator iterator = attributeList.begin();
//                  iterator != attributeList.end(); iterator++)
//             {
//                 // Create attribute string (format: name="value") from the attribute
//                 const std::string attributeString = XmlWriter::createAttributeString(*iterator);
// 
//                 if (!attributeString.empty())
//                 {
//                     // Append a separator and the attribute string to the element string
//                     elementString.append(" ").append(attributeString);
//                 }
//                 else
//                 {
//                     // Error: invalid attribute. Exit loop.
//                     elementString.clear();
//                     break;
//                 }
//             }
//         }
//     }
// 
//     return elementString;
// }
