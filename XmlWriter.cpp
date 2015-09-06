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
#include "Common.h"

using namespace MiniSaxCpp;

/**
 * Constructor
 */
XmlWriter::XmlWriter()
{
    clearDocument();
}

/**
 * Clear XML document
 */
void XmlWriter::clearDocument()
{
    m_state = State_Empty;
    m_xmlDeclarationSet = false;
    m_documentType;
    m_openedElementList.clear();
    m_currentElementInfo = ElementInfo();
    m_attributeNameList.clear();
    m_xmlString.clear();
}

/**
 * Get XML string
 *
 * \return XML string
 * \return Empty string on error
 *
 * \note XML string will only be returned if XML document has been fully completed (root element
 *       has to be ended), otherwise an empty string will be returned.
 */
std::string XmlWriter::getXmlString() const
{
    std::string xmlString;

    if (m_state == State_DocumentEnded)
    {
        xmlString = m_xmlString;
    }

    return xmlString;
}

/**
 * Sets XML Declaration in the XML document
 *
 * \retval true     Success
 * \retval false    Error, XML document is not empty
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
        m_xmlString = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";

        m_xmlDeclarationSet = true;
        m_state = State_DocumentStarted;
        success = true;
    }

    return success;
}

/**
 * Sets Document Type in the XML document
 *
 * \param documentType  Document type name
 *
 * \retval true     Success
 * \retval false    Error
 *
 * \note If the document type name is set then the root element name must match the document type
 *       name!
 */
bool XmlWriter::setDocumentType(const std::string &documentType)
{
    bool success = false;

    if (m_documentType.empty())
    {
        if ((m_state == State_Empty) ||
            (m_state == State_DocumentStarted))
        {
            if (XmlValidator::validateNameString(documentType))
            {
                // Create Document Type string
                m_xmlString.append("<!DOCTYPE ").append(documentType).append(">");

                m_documentType = documentType;
                m_state = State_DocumentStarted;
                success = true;
            }
        }
    }

    return success;
}

/**
 * Add a comment to the XML document
 *
 * \param commentText   Text that should be written to the comment
 *
 * \retval true     Success
 * \retval false    Error
 */
bool XmlWriter::addComment(const std::string &commentText)
{
    bool success = false;
    bool closeStartTag = false;
    State nextState = m_state;

    if (XmlValidator::validateCommentString(commentText))
    {
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
                closeStartTag = true;
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
    }

    if (success)
    {
        if (closeStartTag)
        {
            m_xmlString.append(">");
            m_currentElementInfo.contentEmpty = false;
            m_attributeNameList.clear();
        }

        m_xmlString.append("<!--").append(commentText).append("-->");
        m_state = nextState;
    }

    return success;
}

/**
 * Adds a new processing instruction in the XML document
 *
 * \param piTarget  Processing instruction target name
 * \param piValue   Processing instruction value
 *
 * \retval true     Success
 * \retval false    Error
 */
bool XmlWriter::addProcessingInstruction(const std::string &piTarget, const std::string &piValue)
{
    bool success = false;
    bool closeStartTag = false;
    State nextState = m_state;

    if (XmlValidator::validatePiTargetString(piTarget))
    {
        if (XmlValidator::validatePiValueString(piValue))
        {
            switch (m_state)
            {
                case State_Empty:
                {
                    nextState = State_DocumentStarted;
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

                case State_ElementStarted:
                {
                    // Close start tag of current element
                    closeStartTag = true;
                    nextState = State_InElement;
                    success = true;
                    break;
                }

                default:
                {
                    break;
                }
            }
        }
    }

    if (success)
    {
        if (closeStartTag)
        {
            m_xmlString.append(">");
            m_currentElementInfo.contentEmpty = false;
            m_attributeNameList.clear();
        }

        m_xmlString.append("<?").append(piTarget);

        if (!piValue.empty())
        {
            m_xmlString.append(" ").append(piValue);
        }

        m_xmlString.append("?>");
        m_state = nextState;
    }

    return success;
}

/**
 * Starts a new root element in the XML document
 *
 * \param rootElementName   Root element name
 *
 * \retval true     Success
 * \retval false    Error
 *
 * \note If Document Type is set then the root element name must match it!
 */
bool XmlWriter::startRootElement(const std::string &rootElementName)
{
    bool success = false;

    if (XmlValidator::validateNameString(rootElementName))
    {
        switch (m_state)
        {
            case State_DocumentStarted:
            {
                // No validation of root element name is required if document type is not set
                if (m_documentType.empty())
                {
                    success = true;
                }
                // Check if element name matches the document type
                else if (rootElementName == m_documentType)
                {
                    success = true;
                }
                else
                {
                    // Error, invalid root element name
                }
                break;
            }

            case State_Empty:
            {
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
        // Open start tag
        m_xmlString.append("<").append(rootElementName);

        m_attributeNameList.clear();
        m_currentElementInfo = ElementInfo(rootElementName);
        m_state = State_ElementStarted;
        success = true;
    }

    return success;
}

/**
 * Starts a new child element in the XML document
 *
 * \param elementName   Element name
 *
 * \retval true     Success
 * \retval false    Error
 *
 * \note This method cannot start a root element, to do that use the startRootElement method!
 */
bool XmlWriter::startChildElement(const std::string &elementName)
{
    bool success = false;
    bool closeStartTag = false;

    if (XmlValidator::validateNameString(elementName))
    {
        switch (m_state)
        {
            case State_ElementStarted:
            {
                // Close start tag of current element
                closeStartTag = true;
                success = true;
                break;
            }

            case State_InElement:
            {
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
        if (closeStartTag)
        {
            m_xmlString.append(">");
            m_currentElementInfo.contentEmpty = false;
        }

        // Add current element info to opened element list
        m_openedElementList.push_back(m_currentElementInfo);

        // Open start tag
        m_xmlString.append("<").append(elementName);

        m_attributeNameList.clear();
        m_currentElementInfo = ElementInfo(elementName);
        m_state = State_ElementStarted;
        success = true;
    }

    return success;
}

/**
 * Add the attribute to the current element
 *
 * \param attribute     Attribute
 * \param quotationMark Quotation mark
 *
 * \retval true     Success
 * \retval false    Error
 */
bool XmlWriter::addAttribute(const std::string &name,
                             const std::string &value,
                             const Common::QuotationMark quotationMark)
{
    bool success = false;

    if (m_state == State_ElementStarted)
    {
        if (XmlValidator::validateNameString(name))
        {
            // Check if an attribute with the same name has already been added to the element
            bool attributeNameFound = false;

            for (std::list<std::string>::const_iterator it = m_attributeNameList.begin();
                 it != m_attributeNameList.end();
                 it++)
            {
                const std::string &attributeName = *it;

                if (name == attributeName)
                {
                    // Error, an attribute with the same name is already in the list
                    attributeNameFound = true;
                    break;
                }
            }

            // Attribute names must be unique within an elements start tag
            if (!attributeNameFound)
            {
                if (XmlValidator::validateAttValueString(value, quotationMark))
                {
                    char quotationMarkCharacter;

                    switch (quotationMark)
                    {
                        case Common::QuotationMark_Quote:
                        {
                            quotationMarkCharacter = '"';
                            success = true;
                            break;
                        }

                        case Common::QuotationMark_Apostrophe:
                        {
                            quotationMarkCharacter = '\'';
                            success = true;
                            break;
                        }

                        default:
                        {
                            // Error, invalid quotation mark
                            break;
                        }
                    }

                    if (success)
                    {
                        m_attributeNameList.push_back(name);

                        m_xmlString.append(" ").append(name);
                        m_xmlString.append("=").append(1U, quotationMarkCharacter);
                        m_xmlString.append(value).append(1U, quotationMarkCharacter);
                        success = true;
                    }
                }
            }
        }
    }

    return success;
}

/**
 * Add a text node
 *
 * \param textNode  UTF-8 encoded string
 *
 * \retval true     Success
 * \retval false    Error
 */
bool XmlWriter::addTextNode(const std::string &textNode)
{
    bool success = false;
    bool closeStartTag = false;

    if (XmlValidator::validateTextNodeString(textNode))
    {
        switch (m_state)
        {
            case State_InElement:
            {
                success = true;
                break;
            }

            case State_ElementStarted:
            {
                // Close start tag of current element
                closeStartTag = true;
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
        if (closeStartTag)
        {
            m_xmlString.append(">");
            m_currentElementInfo.contentEmpty = false;
            m_attributeNameList.clear();
        }

        m_xmlString.append(textNode);
        m_state = State_InElement;
        success = true;
    }

    return success;
}

/**
 * Add a CDATA section
 *
 * \param cData UTF-8 encoded string
 *
 * \retval true     Success
 * \retval false    Error
 */
bool XmlWriter::addCDataSection(const std::string &cData)
{
    bool success = false;
    bool closeStartTag = false;

    if (XmlValidator::validateCDataString(cData))
    {
        switch (m_state)
        {
            case State_InElement:
            {
                success = true;
                break;
            }

            case State_ElementStarted:
            {
                // Close start tag of current element
                closeStartTag = true;
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
        if (closeStartTag)
        {
            m_xmlString.append(">");
            m_currentElementInfo.contentEmpty = false;
            m_attributeNameList.clear();
        }

        m_xmlString.append("<![CDATA[").append(cData).append("]]>");
        m_state = State_InElement;
        success = true;
    }

    return success;
}

/**
 *
 * Ends an element in the XML document
 *
 * \param elementName   Element name
 *
 * \retval true     Success
 * \retval false    Error
 */
bool XmlWriter::endElement()
{
    bool success = false;

    if ((m_state == State_ElementStarted) ||
        (m_state == State_InElement))
    {
        // Close the current element
        if (m_currentElementInfo.contentEmpty)
        {
            // Close an empty element
            m_xmlString.append("/>");
        }
        else
        {
            // Close a non-empty element
            m_xmlString.append("</").append(m_currentElementInfo.name).append(">");
        }

        // Discard the current element
        if (m_openedElementList.empty())
        {
            // Currently open element is the root element - close it and end the XML document
            m_currentElementInfo = ElementInfo();
            m_state = State_DocumentEnded;
        }
        else
        {
            // Replace the current element with its parent
            m_currentElementInfo = m_openedElementList.back();
            m_openedElementList.pop_back();
            m_state = State_InElement;
        }

        success = true;
    }

    return success;
}
