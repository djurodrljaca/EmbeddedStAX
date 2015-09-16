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

#include "XmlReader.h"
#include "Utf.h"
#include "XmlValidator.h"

using namespace MiniSaxCpp;

/**
 * Constructor
 */
XmlReader::XmlReader(const size_t dataBufferSize)
    : m_itemParser(dataBufferSize)
{
    clear();
}

/**
 * Clear internal state
 */
void XmlReader::clear()
{
    m_documentState = DocumentState_PrologXmlDeclaration;

    m_itemParser.clear();
    m_parsingState = ParsingState_Idle;
    m_lastParsingResult = ParsingResult_NeedMoreData;

    m_xmlVersion = Common::XmlVersion_None;
    m_xmlEncoding = Common::XmlEncoding_None;
    m_xmlStandalone = Common::XmlStandalone_None;

    m_name.clear();
    m_value.clear();

    m_documentTypeName.clear();

    m_openedElementNameList.clear();
    m_elementAttributeNameList.clear();
}

/**
 * Write data to the XmlReader's data buffer
 *
 * \param data  Data to write
 *
 * \retval true     Success
 * \retval false    Error, data buffer is full
 */
bool XmlReader::writeData(const char data)
{
    return m_itemParser.writeData(data);
}

/**
 * Parse data in the data buffer
 *
 * \return Parsing result
 */
XmlReader::ParsingResult XmlReader::parse()
{
    // TODO: silently recover from error?

    ParsingResult result = ParsingResult_Error;

    // Execute parsing state machine
    bool finishParsing;

    do
    {
        finishParsing = true;
        ParsingState nextState = m_parsingState;

        switch (m_parsingState)
        {
            case ParsingState_Idle:
            {
                nextState = executeParsingStateIdle();

                // Check transitions
                switch (nextState)
                {
                    case ParsingState_ReadingItem:
                    {
                        // Execute another cycle
                        finishParsing = false;
                        break;
                    }

                    default:
                    {
                        // Error, invalid transition
                        nextState = ParsingState_Error;
                        break;
                    }
                }
                break;
            }

            case ParsingState_ReadingItem:
            {
                nextState = executeParsingStateReadingItem();

                // Check transitions
                switch (nextState)
                {
                    case ParsingState_ReadingItem:
                    {
                        // Wait for more data
                        result = ParsingResult_NeedMoreData;
                        break;
                    }

                    case ParsingState_ReadingPiTarget:
                    case ParsingState_ReadingDocumentTypeName:
                    case ParsingState_ReadingCommentText:
                    case ParsingState_ReadingElementName:
                    case ParsingState_ReadingCData:
                    {
                        // Execute another cycle
                        finishParsing = false;
                        break;
                    }

                    default:
                    {
                        // Error, invalid transition
                        nextState = ParsingState_Error;
                        break;
                    }
                }
                break;
            }

            case ParsingState_ReadingPiTarget:
            {
                nextState = executeParsingStateReadingPiTarget();

                // Check transitions
                switch (nextState)
                {
                    case ParsingState_ReadingPiTarget:
                    {
                        // Wait for more data
                        result = ParsingResult_NeedMoreData;
                        break;
                    }

                    case ParsingState_ReadingPiValue:
                    {
                        // Execute another cycle
                        finishParsing = false;
                        break;
                    }

                    default:
                    {
                        // Error, invalid transition
                        nextState = ParsingState_Error;
                        break;
                    }
                }
                break;
            }

            case ParsingState_ReadingPiValue:
            {
                nextState = executeParsingStateReadingPiValue();

                // Check transitions
                switch (nextState)
                {
                    case ParsingState_ReadingPiValue:
                    {
                        // Wait for more data
                        result = ParsingResult_NeedMoreData;
                        break;
                    }

                    case ParsingState_XmlDeclarationRead:
                    {
                        // XML declaration read
                        result = ParsingResult_XmlDeclaration;
                        break;
                    }

                    case ParsingState_ProcessingInstructionRead:
                    {
                        // Processing instruction read
                        result = ParsingResult_ProcessingInstruction;
                        break;
                    }

                    default:
                    {
                        // Error, invalid transition
                        nextState = ParsingState_Error;
                        break;
                    }
                }
                break;
            }

            case ParsingState_ReadingDocumentTypeName:
            {
                nextState = executeParsingStateReadingDocumentTypeName();

                // Check transitions
                switch (nextState)
                {
                    case ParsingState_ReadingDocumentTypeName:
                    {
                        // Wait for more data
                        result = ParsingResult_NeedMoreData;
                        break;
                    }

                    case ParsingState_ReadingDocumentTypeValue:
                    {
                        // Execute another cycle
                        finishParsing = false;
                        break;
                    }

                    default:
                    {
                        // Error, invalid transition
                        nextState = ParsingState_Error;
                        break;
                    }
                }
                break;
            }

            case ParsingState_ReadingDocumentTypeValue:
            {
                nextState = executeParsingStateReadingDocumentTypeValue();

                // Check transitions
                switch (nextState)
                {
                    case ParsingState_ReadingDocumentTypeValue:
                    {
                        // Wait for more data
                        result = ParsingResult_NeedMoreData;
                        break;
                    }

                    case ParsingState_DocumentTypeRead:
                    {
                        // Document type value read
                        result = ParsingResult_DocumentType;
                        break;
                    }

                    default:
                    {
                        // Error, invalid transition
                        nextState = ParsingState_Error;
                        break;
                    }
                }
                break;
            }

            case ParsingState_XmlDeclarationRead:
            case ParsingState_DocumentTypeRead:
            {
                // Initiate reading of an item
                if (m_itemParser.configure(XmlItemParser::Action_ReadItem,
                                           XmlItemParser::Option_IgnoreLeadingWhitespace))
                {
                    nextState = ParsingState_ReadingItem;

                    // Execute another cycle
                    finishParsing = false;
                }
                else
                {
                    // Error
                    nextState = ParsingState_Error;
                }
                break;
            }

            case ParsingState_ReadingCommentText:
            {
                nextState = executeParsingStateReadingCommentText();

                // Check transitions
                switch (nextState)
                {
                    case ParsingState_ReadingCommentText:
                    {
                        // Wait for more data
                        result = ParsingResult_NeedMoreData;
                        break;
                    }

                    case ParsingState_CommentTextRead:
                    {
                        // Comment text read
                        result = ParsingResult_Comment;
                        break;
                    }

                    default:
                    {
                        // Error, invalid transition
                        nextState = ParsingState_Error;
                        break;
                    }
                }
                break;
            }

            case ParsingState_ProcessingInstructionRead:
            case ParsingState_CommentTextRead:
            {
                if (m_documentState == DocumentState_Document)
                {
                    // Initiate reading of a text node
                    if (m_itemParser.configure(XmlItemParser::Action_ReadTextNode))
                    {
                        nextState = ParsingState_ReadingTextNode;

                        // Execute another cycle
                        finishParsing = false;
                    }
                    else
                    {
                        // Error
                        nextState = ParsingState_Error;
                    }
                }
                else
                {
                    // Initiate reading of an item
                    if (m_itemParser.configure(XmlItemParser::Action_ReadItem,
                                               XmlItemParser::Option_IgnoreLeadingWhitespace))
                    {
                        nextState = ParsingState_ReadingItem;

                        // Execute another cycle
                        finishParsing = false;
                    }
                    else
                    {
                        // Error
                        nextState = ParsingState_Error;
                    }
                }
                break;
            }

            case ParsingState_ReadingElementName:
            {
                nextState = executeParsingStateReadingElementName();

                // Check transitions
                switch (nextState)
                {
                    case ParsingState_ReadingElementName:
                    {
                        // Wait for more data
                        result = ParsingResult_NeedMoreData;
                        break;
                    }

                    case ParsingState_ElementNameRead:
                    {
                        // Element name read
                        const size_t size = m_openedElementNameList.size();

                        if (size == 0U)
                        {
                            // Error, invalid list size (this should not be possible!)
                            nextState = ParsingState_Error;
                        }
                        else if (size == 1U)
                        {
                            // Start of root element found
                            result = ParsingResult_StartOfRootElement;
                        }
                        else
                        {
                            // Start of element found
                            result = ParsingResult_StartOfChildElement;
                        }
                        break;
                    }

                    default:
                    {
                        // Error, invalid transition
                        nextState = ParsingState_Error;
                        break;
                    }
                }
                break;
            }

            case ParsingState_ElementNameRead:
            {
                // Initiate reading of an attribute name
                if (m_itemParser.configure(XmlItemParser::Action_ReadAttributeName,
                                           XmlItemParser::Option_IgnoreLeadingWhitespace))
                {
                    nextState = ParsingState_ReadingElementAttributeName;

                    // Execute another cycle
                    finishParsing = false;
                }
                else
                {
                    // Error
                    nextState = ParsingState_Error;
                }
                break;
            }

            case ParsingState_ReadingElementAttributeName:
            {
                nextState = executeParsingStateReadingElementAttributeName();

                // Check transitions
                switch (nextState)
                {
                    case ParsingState_ReadingElementAttributeName:
                    {
                        // Wait for more data
                        result = ParsingResult_NeedMoreData;
                        break;
                    }

                    case ParsingState_ElementAttributeNameRead:
                    {
                        // Execute another cycle
                        finishParsing = false;
                        break;
                    }

                    case ParsingState_ElementStartOfContentRead:
                    {
                        // Execute another cycle
                        finishParsing = false;
                        break;
                    }

                    case ParsingState_ElementEndEmptyRead:
                    {
                        if (m_openedElementNameList.empty())
                        {
                            // Error, no element is open (this situation should never occur!)
                        }
                        else
                        {
                            // End of empty element found
                            if (m_openedElementNameList.empty())
                            {
                                m_documentState = DocumentState_EndOfDocument;
                                result = ParsingResult_EndOfRootElement;
                            }
                            else
                            {
                                result = ParsingResult_EndOfChildElement;
                            }
                        }
                        break;
                    }

                    default:
                    {
                        // Error, invalid transition
                        nextState = ParsingState_Error;
                        break;
                    }
                }
                break;
            }

            case ParsingState_ElementAttributeNameRead:
            {
                // Initiate reading of an attribute value
                if (m_itemParser.configure(XmlItemParser::Action_ReadAttributeValue))
                {
                    nextState = ParsingState_ReadingElementAttributeValue;

                    // Execute another cycle
                    finishParsing = false;
                }
                else
                {
                    // Error
                    nextState = ParsingState_Error;
                }
                break;
            }

            case ParsingState_ReadingElementAttributeValue:
            {
                nextState = executeParsingStateReadingElementAttributeValue();

                // Check transitions
                switch (nextState)
                {
                    case ParsingState_ReadingElementAttributeValue:
                    {
                        // Wait for more data
                        result = ParsingResult_NeedMoreData;
                        break;
                    }

                    case ParsingState_ElementAttributeValueRead:
                    {
                        // Element attribute read
                        result = ParsingResult_ElementAttribute;
                        break;
                    }

                    default:
                    {
                        // Error, invalid transition
                        nextState = ParsingState_Error;
                        break;
                    }
                }
                break;
            }

            case ParsingState_ElementAttributeValueRead:
            {
                // Initiate reading of an attribute name
                if (m_itemParser.configure(XmlItemParser::Action_ReadAttributeName,
                                           XmlItemParser::Option_IgnoreLeadingWhitespace))
                {
                    nextState = ParsingState_ReadingElementAttributeName;

                    // Execute another cycle
                    finishParsing = false;
                }
                else
                {
                    // Error
                    nextState = ParsingState_Error;
                }
                break;
            }

            case ParsingState_ElementEndEmptyRead:
            {
                nextState = ParsingState_Error;

                if (m_documentState == DocumentState_Document)
                {
                    // Initiate reading of an item
                    if (m_itemParser.configure(XmlItemParser::Action_ReadTextNode))
                    {
                        nextState = ParsingState_ReadingTextNode;

                        // Execute another cycle
                        finishParsing = false;
                    }
                    else
                    {
                        // Error
                    }
                }
                else if (m_documentState == DocumentState_EndOfDocument)
                {
                    // Initiate reading of an item
                    if (m_itemParser.configure(XmlItemParser::Action_ReadItem,
                                               XmlItemParser::Option_IgnoreLeadingWhitespace))
                    {
                        nextState = ParsingState_ReadingItem;

                        // Execute another cycle
                        finishParsing = false;
                    }
                    else
                    {
                        // Error
                    }
                }
                else
                {
                    // Error
                }
                break;
            }

            case ParsingState_ReadingTextNode:
            {
                nextState = executeParsingStateReadingTextNode();

                // Check transitions
                switch (nextState)
                {
                    case ParsingState_ReadingTextNode:
                    {
                        // Wait for more data
                        result = ParsingResult_NeedMoreData;
                        break;
                    }

                    case ParsingState_TextNodeRead:
                    {
                        if (m_value.empty())
                        {
                            // Start of another item was found, execute another cycle
                            finishParsing = false;
                        }
                        else
                        {
                            // Text node read
                            result = ParsingResult_TextNode;
                        }
                        break;
                    }

                    default:
                    {
                        // Error, invalid transition
                        nextState = ParsingState_Error;
                        break;
                    }
                }
                break;
            }

            case ParsingState_ReadingCData:
            {
                nextState = executeParsingStateReadingCData();

                // Check transitions
                switch (nextState)
                {
                    case ParsingState_ReadingCData:
                    {
                        // Wait for more data
                        result = ParsingResult_NeedMoreData;
                        break;
                    }

                    case ParsingState_CDataRead:
                    {
                        // CDATA read
                        result = ParsingResult_CData;
                        break;
                    }

                    default:
                    {
                        // Error, invalid transition
                        nextState = ParsingState_Error;
                        break;
                    }
                }
                break;
            }

            case ParsingState_TextNodeRead:
            {
                // Initiate reading of an item
                if (m_itemParser.configure(XmlItemParser::Action_ReadItem))
                {
                    nextState = ParsingState_ReadingItem;

                    // Execute another cycle
                    finishParsing = false;
                }
                else
                {
                    // Error
                    nextState = ParsingState_Error;
                }
                break;
            }

            case ParsingState_ElementStartOfContentRead:
            case ParsingState_CDataRead:
            {
                // Initiate reading of a text node
                if (m_itemParser.configure(XmlItemParser::Action_ReadTextNode))
                {
                    nextState = ParsingState_ReadingTextNode;

                    // Execute another cycle
                    finishParsing = false;
                }
                else
                {
                    // Error
                    nextState = ParsingState_Error;
                }
                break;
            }

            default:
            {
                // Error, invalid state value
                nextState = ParsingState_Error;
                break;
            }
        }

        // Check next parsing state
        if (nextState == ParsingState_Error)
        {
            m_documentState = DocumentState_Error;
            finishParsing = true;
        }

        m_parsingState = nextState;
    }
    while(!finishParsing);

    // Save last parsing result
    m_lastParsingResult = result;
    return result;
}

/**
 * Get last parsing result
 *
 * \return Last parsing result
 */
XmlReader::ParsingResult XmlReader::getLastParsingResult()
{
    return m_lastParsingResult;
}

/**
 * Get name string
 *
 * \return Name string
 *
 * \note Context for this value can be determined from the parsing result
 */
std::string XmlReader::getName() const
{
    return m_name;
}

/**
 * Get value string
 *
 * \return Value string
 *
 * \note Context for this value can be determined from the parsing result
 */
std::string XmlReader::getValue() const
{
    return m_value;
}

/**
 * Checks if XML declaration is set in the document
 *
 * \retval true     XML declaration is set
 * \retval false    XML declaration is not set
 */
bool XmlReader::isXmlDeclarationSet() const
{
    bool valid = false;

    if ((m_xmlVersion != Common::XmlVersion_None) &&
        (m_xmlEncoding != Common::XmlEncoding_None) &&
        (m_xmlStandalone != Common::XmlStandalone_None))
    {
        valid = true;
    }

    return valid;
}

/**
 * Checks if XML declaration is set in the document
 *
 * \retval true     XML declaration is supported
 * \retval false    XML declaration is not supported
 */
bool XmlReader::isXmlDeclarationSupported() const
{
    bool supported = true;

    if ((m_xmlVersion == Common::XmlVersion_Unknown) &&
        (m_xmlEncoding == Common::XmlEncoding_Unknown) &&
        (m_xmlStandalone == Common::XmlStandalone_None))
    {
        supported = false;
    }

    return supported;
}

/**
 * Get "version" from the documents XML declaration
 *
 * \return Version value
 */
Common::XmlVersion XmlReader::getXmlVersion() const
{
    return m_xmlVersion;
}

/**
 * Get "encoding" from the documents XML declaration
 *
 * \return Encoding value
 */
Common::XmlEncoding XmlReader::getXmlEncoding() const
{
    return m_xmlEncoding;
}

/**
 * Get "standalone" value from the documents XML declaration
 *
 * \return Standalone value
 */
Common::XmlStandalone XmlReader::getXmlStandalone() const
{
    return m_xmlStandalone;
}

/**
 * Execute parsing state machine state: Waiting for start of item
 *
 * \retval ParsingState_WaitingForStartOfItem   Waiting for more data
 * \retval ParsingState_ReadingItemType         Start of item found
 */
XmlReader::ParsingState XmlReader::executeParsingStateIdle()
{
    ParsingState nextState = ParsingState_Error;

    // Prepare parsing option
    XmlItemParser::Option option = XmlItemParser::Option_None;

    if (m_documentState == DocumentState_PrologDocumentType)
    {
        option = XmlItemParser::Option_IgnoreLeadingWhitespace;
    }

    // Initiate reading of an item
    if (m_itemParser.configure(XmlItemParser::Action_ReadItem, option))
    {
        nextState = ParsingState_ReadingItem;
    }

    return nextState;
}

/**
 * Execute parsing state machine state: Reading item
 *
 * \retval ParsingState_ReadingItem             Waiting for more data
 * \retval ParsingState_ReadingPiTarget         Processing instruction item found
 * \retval ParsingState_ReadingDocumentTypeName Document type item found
 * \retval ParsingState_ReadingCommentText      Comment item found
 * \retval ParsingState_ReadingElementName      Start of element found
 * \retval ParsingState_Error                   Error occured
 *
 * \todo Add other return values!
 */
XmlReader::ParsingState XmlReader::executeParsingStateReadingItem()
{
    ParsingState nextState = ParsingState_Error;
    bool finishParsing = false;

    do
    {
        XmlItemParser::Result result = m_itemParser.execute();

        switch (result)
        {
            case XmlItemParser::Result_Success:
            {
                // Handle item type
                switch (m_itemParser.getItemType())
                {
                    case XmlItemParser::ItemType_Whitespace:
                    {
                        // Check document state
                        if (m_documentState == DocumentState_PrologXmlDeclaration)
                        {
                            // XML declaration is not at the start of the XML string. For a valid
                            // XML document the XML declaration has to be located at the start of
                            // the XML string otherwise it is not allowed to occur in the XML
                            // string. Change the document state accordingly.
                            m_documentState = DocumentState_PrologDocumentType;
                        }

                        // Continue parsing, but ignore leading whitespace
                        if (m_itemParser.configure(XmlItemParser::Action_ReadItem,
                                                   XmlItemParser::Option_IgnoreLeadingWhitespace))
                        {
                            nextState = ParsingState_ReadingItem;
                        }
                        else
                        {
                            // Error, failed to continue reading item
                            finishParsing = true;
                        }
                        break;
                    }

                    case XmlItemParser::ItemType_ProcessingInstruction:
                    {
                        // Parse PITarget
                        if (m_itemParser.configure(XmlItemParser::Action_ReadName))
                        {
                            nextState = ParsingState_ReadingPiTarget;
                        }
                        else
                        {
                            // Error, failed to continue reading processing instruction
                        }

                        finishParsing = true;
                        break;
                    }

                    case XmlItemParser::ItemType_DocumentType:
                    {
                        // Parse document type name
                        if (m_itemParser.configure(XmlItemParser::Action_ReadName,
                                                   XmlItemParser::Option_IgnoreLeadingWhitespace))
                        {
                            nextState = ParsingState_ReadingDocumentTypeName;
                        }
                        else
                        {
                            // Error, failed to continue reading document type
                        }

                        finishParsing = true;
                        break;
                    }

                    case XmlItemParser::ItemType_Comment:
                    {
                        // Parse comment text
                        if (m_itemParser.configure(XmlItemParser::Action_ReadCommentText))
                        {
                            nextState = ParsingState_ReadingCommentText;
                        }
                        else
                        {
                            // Error, failed to continue reading comment text
                        }

                        finishParsing = true;
                        break;
                    }

                    case XmlItemParser::ItemType_StartOfElement:
                    {
                        // Parse element name
                        if (m_itemParser.configure(XmlItemParser::Action_ReadName))
                        {
                            nextState = ParsingState_ReadingElementName;
                        }
                        else
                        {
                            // Error, failed to continue reading element name
                        }

                        finishParsing = true;
                        break;
                    }

                    case XmlItemParser::ItemType_CData:
                    {
                        // Parse CDATA
                        if (m_itemParser.configure(XmlItemParser::Action_ReadCData))
                        {
                            nextState = ParsingState_ReadingCData;
                        }
                        else
                        {
                            // Error, failed to continue reading CDATA
                        }

                        finishParsing = true;
                        break;
                    }

                    default:
                    {
                        // Error, invalid type
                        finishParsing = true;
                        break;
                    }
                }
                break;
            }

            case XmlItemParser::Result_NeedMoreData:
            {
                // Wait for more data
                nextState = ParsingState_ReadingItem;
                finishParsing = true;
                break;
            }

            default:
            {
                // Error
                finishParsing = true;
                break;
            }
        }
    }
    while (!finishParsing);

    return nextState;
}

/**
 * Execute parsing state machine state: Reading PITarget
 *
 * \retval ParsingState_ReadingPiTarget Waiting for more data
 * \retval ParsingState_ReadingPiValue  PITarget read
 * \retval ParsingState_Error           Error occured
 */
XmlReader::ParsingState XmlReader::executeParsingStateReadingPiTarget()
{
    ParsingState nextState = ParsingState_Error;
    XmlItemParser::Result result = m_itemParser.execute();

    switch (result)
    {
        case XmlItemParser::Result_Success:
        {
            // Check termination character
            const uint32_t terminationCharacter = m_itemParser.getTerminationCharacter();

            if ((terminationCharacter == (uint32_t)'?') ||
                (XmlValidator::isWhitespace(terminationCharacter)))
            {
                // PITarget read
                m_name = getItemParserValue();

                if (m_name.empty())
                {
                    // Error, failed to get a valid PITarget
                }
                else
                {
                    // Read processing instruction value
                    if (m_itemParser.configure(XmlItemParser::Action_ReadPiValue,
                                               XmlItemParser::Option_IgnoreLeadingWhitespace))
                    {
                        m_value.clear();
                        nextState = ParsingState_ReadingPiValue;
                    }
                }
            }
            else
            {
                // Error, invalid termination character
            }
            break;
        }

        case XmlItemParser::Result_NeedMoreData:
        {
            // Wait for more data
            nextState = ParsingState_ReadingPiTarget;
            break;
        }

        default:
        {
            // Error
            break;
        }
    }

    return nextState;
}

/**
 * Execute parsing state machine state: Reading processing instruction value
 *
 * \retval ParsingState_ReadingPiValue              Waiting for more data
 * \retval ParsingState_XmlDeclarationRead          XML declaration read
 * \retval ParsingState_ProcessingInstructionRead   Processing instruction read
 * \retval ParsingState_Error                       Error occured
 */
XmlReader::ParsingState XmlReader::executeParsingStateReadingPiValue()
{
    ParsingState nextState = ParsingState_Error;
    XmlItemParser::Result result = m_itemParser.execute();

    switch (result)
    {
        case XmlItemParser::Result_Success:
        {
            // Processing instruction value read
            bool success = false;
            m_value = getItemParserValue(&success);

            if (success)
            {
                // End of processing instruction found, evaluate it (check if XML declaration)
                nextState = evaluateProcessingInstruction();
            }
            else
            {
                // Error, failed to get a valid processing instruction value
            }
            break;
        }

        case XmlItemParser::Result_NeedMoreData:
        {
            // Wait for more data
            nextState = ParsingState_ReadingPiValue;
            break;
        }

        default:
        {
            // Error
            break;
        }
    }

    return nextState;
}

/**
 * Execute parsing state machine state: Reading document type name
 *
 * \retval ParsingState_ReadingDocumentTypeName     Waiting for more data
 * \retval ParsingState_ReadingDocumentTypeValue    Document type name read
 * \retval ParsingState_Error                       Error occured
 */
XmlReader::ParsingState XmlReader::executeParsingStateReadingDocumentTypeName()
{
    ParsingState nextState = ParsingState_Error;
    XmlItemParser::Result result = m_itemParser.execute();

    switch (result)
    {
        case XmlItemParser::Result_Success:
        {
            // Check termination character
            const uint32_t terminationCharacter = m_itemParser.getTerminationCharacter();

            if ((terminationCharacter == (uint32_t)'>') ||
                (XmlValidator::isWhitespace(terminationCharacter)))
            {
                // Document type name read
                m_name = getItemParserValue();

                if (m_name.empty())
                {
                    // Error, failed to get a valid document type name
                }
                else
                {
                    // Read document type value
                    if (m_itemParser.configure(XmlItemParser::Action_ReadDocumentTypeValue,
                                               XmlItemParser::Option_IgnoreLeadingWhitespace))
                    {
                        m_value.clear();
                        nextState = ParsingState_ReadingDocumentTypeValue;
                    }
                }
            }
            else
            {
                // Error, invalid termination character
            }
            break;
        }

        case XmlItemParser::Result_NeedMoreData:
        {
            // Wait for more data
            nextState = ParsingState_ReadingDocumentTypeName;
            break;
        }

        default:
        {
            // Error
            break;
        }
    }

    return nextState;
}

/**
 * Execute parsing state machine state: Reading processing instruction value
 *
 * \retval ParsingState_ReadingDocumentTypeValue    Waiting for more data
 * \retval ParsingState_DocumentTypeRead            Document type read
 * \retval ParsingState_Error                       Error occured
 */
XmlReader::ParsingState XmlReader::executeParsingStateReadingDocumentTypeValue()
{
    ParsingState nextState = ParsingState_Error;
    XmlItemParser::Result result = m_itemParser.execute();

    switch (result)
    {
        case XmlItemParser::Result_Success:
        {
            // Document type value read
            bool success = false;
            m_value = getItemParserValue(&success);

            if (success)
            {
                // Check document state
                if ((m_documentState == DocumentState_PrologXmlDeclaration) ||
                    (m_documentState == DocumentState_PrologDocumentType))
                {
                    // XML Document Type can be located either at the start of the XML string or
                    // between a XML declaration and a XML element.
                    m_documentState = DocumentState_PrologOther;

                    // Valid document type
                    m_documentTypeName = m_name;
                    nextState = ParsingState_DocumentTypeRead;
                }
            }
            else
            {
                // Error, failed to get a valid document type value
            }
            break;
        }

        case XmlItemParser::Result_NeedMoreData:
        {
            // Wait for more data
            nextState = ParsingState_ReadingDocumentTypeValue;
            break;
        }

        default:
        {
            // Error
            break;
        }
    }

    return nextState;
}

/**
 * Execute parsing state machine state: Reading processing instruction value
 *
 * \retval ParsingState_ReadingCommentText  Waiting for more data
 * \retval ParsingState_CommentTextRead     Document type read
 * \retval ParsingState_Error               Error occured
 */
XmlReader::ParsingState XmlReader::executeParsingStateReadingCommentText()
{
    ParsingState nextState = ParsingState_Error;
    XmlItemParser::Result result = m_itemParser.execute();

    switch (result)
    {
        case XmlItemParser::Result_Success:
        {
            // Document type value read
            bool success = false;
            m_value = getItemParserValue(&success);

            if (success)
            {
                // Check document state
                if (m_documentState == DocumentState_PrologXmlDeclaration)
                {
                    // XML declaration is not at the start of the XML string. For a valid XML
                    // document the XML declaration has to be located at the start of the XML string
                    // otherwise it is not allowed to occur in the XML string.
                    m_documentState = DocumentState_PrologDocumentType;
                }

                nextState = ParsingState_CommentTextRead;
            }
            else
            {
                // Error, failed to get a valid comment text
            }
            break;
        }

        case XmlItemParser::Result_NeedMoreData:
        {
            // Wait for more data
            nextState = ParsingState_ReadingCommentText;
            break;
        }

        default:
        {
            // Error
            break;
        }
    }

    return nextState;
}

/**
 * Execute parsing state machine state: Reading element name
 *
 * \retval ParsingState_ReadingElementName  Waiting for more data
 * \retval ParsingState_ElementNameRead     Element name read
 * \retval ParsingState_Error               Error occured
 */
XmlReader::ParsingState XmlReader::executeParsingStateReadingElementName()
{
    ParsingState nextState = ParsingState_Error;
    XmlItemParser::Result result = m_itemParser.execute();

    switch (result)
    {
        case XmlItemParser::Result_Success:
        {
            // Check termination character
            const uint32_t terminationCharacter = m_itemParser.getTerminationCharacter();

            if ((terminationCharacter == (uint32_t)'>') ||
                (terminationCharacter == (uint32_t)'/') ||
                (XmlValidator::isWhitespace(terminationCharacter)))
            {
                // Element name read
                m_name = getItemParserValue();

                if (m_name.empty())
                {
                    // Error, failed to get a valid element name
                }
                else
                {
                    bool error = false;

                    // Check for root element
                    if (m_openedElementNameList.empty())
                    {
                        // Validate root element name with document type name
                        if (!m_documentTypeName.empty())
                        {
                            if (m_name != m_documentTypeName)
                            {
                                // Error, invalid root element name
                                error = true;
                            }
                        }

                        // Check document state
                        if ((m_documentState == DocumentState_PrologXmlDeclaration) ||
                            (m_documentState == DocumentState_PrologDocumentType) ||
                            (m_documentState == DocumentState_PrologOther))
                        {
                            // Root element starts the document
                            m_documentState = DocumentState_Document;
                        }
                        else
                        {
                            // Error, invalid document state
                            error = true;
                        }
                    }

                    // Add the element to the opened element list
                    if (!error)
                    {
                        m_openedElementNameList.push_back(m_name);
                        m_elementAttributeNameList.clear();
                        nextState = ParsingState_ElementNameRead;
                    }
                }
            }
            else
            {
                // Error, invalid termination character
            }
            break;
        }

        case XmlItemParser::Result_NeedMoreData:
        {
            // Wait for more data
            nextState = ParsingState_ReadingElementName;
            break;
        }

        default:
        {
            // Error
            break;
        }
    }

    return nextState;
}

/**
 * Execute parsing state machine state: Reading element attribute name
 *
 * \retval ParsingState_ReadingElementAttributeName     Waiting for more data
 * \retval ParsingState_ElementAttributeNameRead        Element attribute name read
 * \retval ParsingState_ReadingElementStartOfContent    Start of element's content found
 * \retval ParsingState_ReadingElementEndEmpty          End of empty element read
 * \retval ParsingState_Error                           Error occured
 */
XmlReader::ParsingState XmlReader::executeParsingStateReadingElementAttributeName()
{
    ParsingState nextState = ParsingState_Error;
    XmlItemParser::Result result = m_itemParser.execute();

    switch (result)
    {
        case XmlItemParser::Result_Success:
        {
            // Check item type
            const XmlItemParser::ItemType itemType = m_itemParser.getItemType();

            switch (itemType)
            {
                case XmlItemParser::ItemType_ElementAttribute:
                {
                    m_name = getItemParserValue();

                    if (m_name.empty())
                    {
                        // Error, empty name
                    }
                    else
                    {
                        // Attribute name was read, check if an attribute with the same name was already
                        // read in the current element
                        bool nameFound = false;
                        std::list<std::string>::const_iterator iterator;

                        for (iterator = m_elementAttributeNameList.begin();
                             iterator != m_elementAttributeNameList.end();
                             iterator++)
                        {
                            const std::string &attributeName = *iterator;

                            if (m_name == attributeName)
                            {
                                nameFound = true;
                                break;
                            }
                        }

                        if (nameFound)
                        {
                            // Error, an attribute with the same name was already found in the current
                            // element
                        }
                        else
                        {
                            // Attribute name read
                            m_elementAttributeNameList.push_back(m_name);
                            nextState = ParsingState_ElementAttributeNameRead;
                        }
                    }
                    break;
                }

                case XmlItemParser::ItemType_StartOfElementContent:
                {
                    // Start of element content found
                    m_elementAttributeNameList.clear();
                    nextState = ParsingState_ElementStartOfContentRead;
                    break;
                }

                case XmlItemParser::ItemType_StartOfEmptyElement:
                {
                    if (m_openedElementNameList.empty())
                    {
                        // Error, the list should not be empty here (start of element had to be
                        // found before we came to this state!)
                    }
                    else
                    {
                        // End of empty element read
                        m_name = m_openedElementNameList.back();
                        m_openedElementNameList.pop_back();

                        m_elementAttributeNameList.clear();
                        nextState = ParsingState_ElementEndEmptyRead;
                    }
                    break;
                }

                default:
                {
                    // Error
                    break;
                }
            }
            break;
        }

        case XmlItemParser::Result_NeedMoreData:
        {
            // Wait for more data
            nextState = ParsingState_ReadingElementAttributeName;
            break;
        }

        default:
        {
            // Error
            break;
        }
    }

    return nextState;
}

/**
 * Execute parsing state machine state: Reading element attribute value
 *
 * \retval ParsingState_ReadingElementAttributeValue    Waiting for more data
 * \retval ParsingState_ElementAttributeValueRead       Element attribute value read
 * \retval ParsingState_Error                           Error occured
 */
XmlReader::ParsingState XmlReader::executeParsingStateReadingElementAttributeValue()
{
    ParsingState nextState = ParsingState_Error;
    XmlItemParser::Result result = m_itemParser.execute();

    switch (result)
    {
        case XmlItemParser::Result_Success:
        {
            // Get attribute value
            bool success = false;
            m_value = getItemParserValue(&success);

            if (success)
            {
                // Attribute value read
                nextState = ParsingState_ElementAttributeValueRead;
            }
            else
            {
                // Error, failed to read item parser value
            }
            break;
        }

        case XmlItemParser::Result_NeedMoreData:
        {
            // Wait for more data
            nextState = ParsingState_ReadingElementAttributeValue;
            break;
        }

        default:
        {
            // Error
            break;
        }
    }

    return nextState;
}

/**
 * Execute parsing state machine state: Reading text node
 *
 * \retval ParsingState_ReadingTextNode Waiting for more data
 * \retval ParsingState_TextNodeRead    Text node read
 * \retval ParsingState_Error           Error occured
 */
XmlReader::ParsingState XmlReader::executeParsingStateReadingTextNode()
{
    ParsingState nextState = ParsingState_Error;
    XmlItemParser::Result result = m_itemParser.execute();

    switch (result)
    {
        case XmlItemParser::Result_Success:
        {
            // Get text node
            bool success = false;
            m_value = getItemParserValue(&success);

            if (success)
            {
                // Text node was read
                nextState = ParsingState_TextNodeRead;
            }
            else
            {
                // Error, failed to read item parser value
            }
            break;
        }

        case XmlItemParser::Result_NeedMoreData:
        {
            // Wait for more data
            nextState = ParsingState_ReadingTextNode;
            break;
        }

        default:
        {
            // Error
            break;
        }
    }

    return nextState;
}

/**
 * Execute parsing state machine state: Reading CDATA
 *
 * \retval ParsingState_ReadingCData    Waiting for more data
 * \retval ParsingState_CDataRead       CDATA read
 * \retval ParsingState_Error           Error occured
 */
XmlReader::ParsingState XmlReader::executeParsingStateReadingCData()
{
    ParsingState nextState = ParsingState_Error;
    XmlItemParser::Result result = m_itemParser.execute();

    switch (result)
    {
        case XmlItemParser::Result_Success:
        {
            // Get CDATA
            bool success = false;
            m_value = getItemParserValue(&success);

            if (success)
            {
                // CDATA was read
                nextState = ParsingState_CDataRead;
            }
            else
            {
                // Error, failed to read item parser value
            }
            break;
        }

        case XmlItemParser::Result_NeedMoreData:
        {
            // Wait for more data
            nextState = ParsingState_ReadingCData;
            break;
        }

        default:
        {
            // Error
            break;
        }
    }

    return nextState;
}

/**
 * Get the value from item parser and convert it to UTF-8 encoded string
 *
 * \param[out] success  Optional output parameter for signaling success or failure
 *
 * \return UTF-8 encoded item parser value or an empty string in case of an error
 */
std::string XmlReader::getItemParserValue(bool *success) const
{
    const UnicodeString itemParserValue = m_itemParser.getValue();
    const std::string value = Utf8::toUtf8(itemParserValue);

    if (success != NULL)
    {
        *success = true;

        if (!itemParserValue.empty() && value.empty())
        {
            *success = false;
        }
    }

    return value;
}

/**
 * Evaluate processing instruction
 *
 * \retval ParsingResult_XmlDeclaration         Processing instruction is a XML Declaration
 * \retval ParsingResult_ProcessingInstruction  Processing instruction is not known
 * \retval ParsingResult_Error                  Error
 */
XmlReader::ParsingState XmlReader::evaluateProcessingInstruction()
{
    ParsingState nextState = ParsingState_ProcessingInstructionRead;

    // Check for XML declaration
    if (m_name.size() == 3U)
    {
        if (((m_name.at(0U) == 'x') || (m_name.at(0U) == 'X')) &&
            ((m_name.at(1U) == 'm') || (m_name.at(1U) == 'M')) &&
            ((m_name.at(2U) == 'l') || (m_name.at(2U) == 'L')))
        {
            // Check document state
            if (m_documentState == DocumentState_PrologXmlDeclaration)
            {
                // XML declaration is at the start of the XML string. For a valid XML document the
                // XML declaration has to be located at the start of the XML string otherwise it is
                // not allowed to occur in the XML string.

                // Parse XML Declaration from the processing instruction's value
                if (parseXmlDeclaration())
                {
                    nextState = ParsingState_XmlDeclarationRead;
                }
                else
                {
                    // Error, problem parsing XML Declaration
                    nextState = ParsingState_Error;
                }
            }
            else
            {
                // Error, either multiple XML Declarations were found or the XML Declaration is not
                // located at the start of the XML document
                nextState = ParsingState_Error;
            }
        }
    }

    if (nextState != ParsingState_Error)
    {
        // Check document state
        if (m_documentState == DocumentState_PrologXmlDeclaration)
        {
            // In case of both a PI and a XML Declaration the document state changes
            // from "Prolog: XML Declaration" to "Prolog: Document Type"
            m_documentState = DocumentState_PrologDocumentType;
        }
    }

    return nextState;
}

/**
 * Parse XML Declaration
 *
 * \retval true     Success
 * \retval false    Error
 *
 * \note XML Declaration will be parsed from the m_value string.
 */
bool XmlReader::parseXmlDeclaration()
{
    bool success = true;
    Common::XmlVersion xmlVersion = Common::XmlVersion_None;
    Common::XmlEncoding xmlEncoding = Common::XmlEncoding_None;
    Common::XmlStandalone xmlStandalone = Common::XmlStandalone_None;
    size_t position = 0U;

    while (success && (position < m_value.size()))
    {
        // Find start of name
        for (; position < m_value.size(); position++)
        {
            const char value = m_value.at(position);

            if (XmlValidator::isWhitespace((uint32_t)value) == false)
            {
                break;
            }
        }

        if (position < m_value.size())
        {
            // Read attribute name
            enum Attribute
            {
                Attribute_None,
                Attribute_Version,
                Attribute_Encoding,
                Attribute_Standalone,
            };

            Attribute attribute = Attribute_None;

            if (success)
            {
                const std::string nameVersion = "version";
                const std::string nameEncoding = "encoding";
                const std::string nameStandalone = "standalone";

                if (m_value.compare(position,
                                    nameVersion.size(),
                                    nameVersion,
                                    0U,
                                    nameVersion.size()) == 0)
                {
                    // Validate position of the parameter
                    if ((xmlVersion == Common::XmlVersion_None) &&
                        (xmlEncoding == Common::XmlEncoding_None) &&
                        (xmlStandalone == Common::XmlStandalone_None))
                    {
                        // Version comes before the other two (optional) attributes
                        position = position + nameVersion.size();
                        attribute = Attribute_Version;
                    }
                    else
                    {
                        // Error, version must be the first attribute
                        success = false;
                    }
                }
                else if (m_value.compare(position,
                                         nameEncoding.size(),
                                         nameEncoding,
                                         0U,
                                         nameEncoding.size()) == 0)
                {
                    // Validate position of the parameter
                    if ((xmlVersion != Common::XmlVersion_None) &&
                        (xmlEncoding == Common::XmlEncoding_None) &&
                        (xmlStandalone == Common::XmlStandalone_None))
                    {
                        // Encoding is an optional attribute and, if present, it must come after version
                        // and before standalone
                        position = position + nameEncoding.size();
                        attribute = Attribute_Encoding;
                    }
                    else
                    {
                        // Error, version must be the first attribute
                        success = false;
                    }
                }
                else if (m_value.compare(position,
                                         nameStandalone.size(),
                                         nameStandalone,
                                         0U,
                                         nameStandalone.size()) == 0)
                {
                    // Validate position of the parameter
                    if ((xmlVersion != Common::XmlVersion_None) &&
                        (xmlStandalone == Common::XmlStandalone_None))
                    {
                        // Standalone is an optional attribute and, if present, it must come after
                        // version and, if present, after encoding
                        position = position + nameStandalone.size();
                        attribute = Attribute_Standalone;
                    }
                    else
                    {
                        // Error, version must be the first attribute
                        success = false;
                    }
                }
                else
                {
                    // Error
                    success = false;
                }

                // Find start of value
                if (success)
                {
                    success = false;

                    while (position < m_value.size())
                    {
                        const char value = m_value.at(position);

                        if (value == '=')
                        {
                            // Found start of value, point to next character
                            position++;
                            success = true;
                            break;
                        }
                        else if (XmlValidator::isWhitespace((uint32_t)value))
                        {
                            // Ignore whitespace
                        }
                        else
                        {
                            // Error, invalid character
                            break;
                        }
                    }
                }
            }

            // Get quote character
            char quote = '\0';

            if (success)
            {
                success = false;

                while (position < m_value.size())
                {
                    const char value = m_value.at(position);

                    if (value == '"')
                    {
                        // Found quote character, point to next character
                        quote = '"';
                        position++;
                        success = true;
                        break;
                    }
                    else if (value == '\'')
                    {
                        // Found quote character, point to next character
                        quote = '\'';
                        position++;
                        success = true;
                        break;
                    }
                    else
                    {
                        // Error, invalid character
                        break;
                    }
                }
            }

            // Read attribute's value
            std::string attributeValue;

            if (success)
            {
                success = false;
                size_t startPosition = position;

                for (; position < m_value.size(); position++)
                {
                    const char value = m_value.at(position);

                    if (value == quote)
                    {
                        // Found end of value, point to next character and save the value
                        const size_t size = position - startPosition;
                        attributeValue = m_value.substr(startPosition, size);

                        position++;
                        success = true;
                        break;
                    }
                }
            }

            // Evaluate value
            if (success)
            {
                switch (attribute)
                {
                    case Attribute_Version:
                    {
                        // Check for valid versions
                        if (attributeValue == "1.0")
                        {
                            xmlVersion = Common::XmlVersion_v1_0;
                        }
                        else
                        {
                            xmlVersion = Common::XmlVersion_Unknown;
                        }
                        break;
                    }

                    case Attribute_Encoding:
                    {
                        // Check for valid encodings
                        if (((attributeValue.at(0U) == 'U') || (attributeValue.at(0U) == 'u')) &&
                            ((attributeValue.at(1U) == 'T') || (attributeValue.at(1U) == 't')) &&
                            ((attributeValue.at(2U) == 'F') || (attributeValue.at(2U) == 'f')) &&
                            (attributeValue.at(3U) == '-') &&
                            (attributeValue.at(4U) == '8'))
                        {
                            xmlEncoding = Common::XmlEncoding_Utf8;
                        }
                        else
                        {
                            xmlEncoding = Common::XmlEncoding_Unknown;
                        }
                        break;
                    }

                    case Attribute_Standalone:
                    {
                        // Check for valid standalone
                        if (attributeValue == "yes")
                        {
                            xmlStandalone = Common::XmlStandalone_Yes;
                        }
                        else if (attributeValue == "no")
                        {
                            xmlStandalone = Common::XmlStandalone_No;
                        }
                        else
                        {
                            xmlStandalone = Common::XmlStandalone_Unknown;
                        }
                        break;
                    }

                    default:
                    {
                        // Error, invalid name
                        success = false;
                        break;
                    }
                }
            }
        }
    }

    if (success)
    {
        m_xmlVersion = xmlVersion;
        m_xmlEncoding = xmlEncoding;
        m_xmlStandalone = xmlStandalone;
    }

    return success;
}
