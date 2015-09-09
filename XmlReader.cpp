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
    m_parsingState = ParsingState_WaitingForStartOfItem;
    m_lastParsingResult = ParsingResult_NeedMoreData;

    m_xmlVersion = Common::XmlVersion_None;
    m_xmlEncoding = Common::XmlEncoding_None;
    m_xmlStandalone = Common::XmlStandalone_None;

    m_name.clear();
    m_value.clear();
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
            case ParsingState_WaitingForStartOfItem:
            {
                nextState = executeParsingStateWaitingForStartOfItem();

                // Check transitions
                switch (nextState)
                {
                    case ParsingState_WaitingForStartOfItem:
                    {
                        // Wait for more data
                        result = ParsingResult_NeedMoreData;
                        break;
                    }

                    case ParsingState_ReadingItemType:
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

            case ParsingState_ReadingItemType:
            {
                nextState = executeParsingStateReadingItemType();

                // Check transitions
                switch (nextState)
                {
                    case ParsingState_ReadingItemType:
                    {
                        // Wait for more data
                        result = ParsingResult_NeedMoreData;
                        break;
                    }

                    case ParsingState_PiTarget:
                    case ParsingState_ElementName:
                    case ParsingState_Comment:
                    case ParsingState_DocumentTypeName:
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

            case ParsingState_PiTarget:
            {
                nextState = executeParsingStatePiTarget();

                // Check transitions
                switch (nextState)
                {
                    case ParsingState_PiTarget:
                    {
                        // Wait for more data
                        result = ParsingResult_NeedMoreData;
                        break;
                    }

                    case ParsingState_PiValue:
                    case ParsingState_PiEnd:
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

            case ParsingState_PiValue:
            {
                nextState = executeParsingStatePiValue();

                // Check transitions
                switch (nextState)
                {
                    case ParsingState_PiValue:
                    {
                        // Wait for more data
                        result = ParsingResult_NeedMoreData;
                        break;
                    }

                    case ParsingState_PiEnd:
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

            case ParsingState_PiEnd:
            {
                ParsingResult newResult = ParsingResult_Error;
                nextState = executeParsingStatePiEnd(&newResult);

                // Check transitions
                switch (nextState)
                {
                    case ParsingState_PiEnd:
                    {
                        // Wait for more data
                        result = ParsingResult_NeedMoreData;
                        break;
                    }

                    case ParsingState_WaitingForStartOfItem:
                    {
                        // Parsing is finished, check if result is valid
                        if ((newResult == ParsingResult_XmlDeclaration) ||
                            (newResult == ParsingResult_ProcessingInstruction))
                        {
                            // Set new result
                            result = newResult;
                        }
                        else
                        {
                            // Error, invalid result
                            nextState = ParsingState_Error;
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
 * Checks if XML Declaration is set in the document
 *
 * \retval true     XML Declaration is set
 * \retval false    XML Declaration is not set
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
 * Checks if XML Declaration is set in the document
 *
 * \retval true     XML Declaration is supported
 * \retval false    XML Declaration is not supported
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
 * Get "version" from the documents XML Declaration
 *
 * \return Version value
 */
Common::XmlVersion XmlReader::getXmlVersion() const
{
    return m_xmlVersion;
}

/**
 * Get "encoding" from the documents XML Declaration
 *
 * \return Encoding value
 */
Common::XmlEncoding XmlReader::getXmlEncoding() const
{
    return m_xmlEncoding;
}

/**
 * Get "standalone" value from the documents XML Declaration
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
XmlReader::ParsingState XmlReader::executeParsingStateWaitingForStartOfItem()
{
    ParsingState nextState = ParsingState_WaitingForStartOfItem;
    bool finishParsing = false;

    do
    {
        XmlItemParser::Option option = XmlItemParser::Option_None;

        if (m_documentState == DocumentState_PrologDocumentType)
        {
            option = XmlItemParser::Option_IgnoreWhitespace;
        }

        XmlItemParser::Result itemParserResult = m_itemParser.parseStartOfItem(option);

        switch (itemParserResult)
        {
            case XmlItemParser::Result_Success:
            {
                const uint32_t terminationCharacter = m_itemParser.getTerminationCharacter();

                if (terminationCharacter == (uint32_t)'<')
                {
                    // Start of item found, try to read the the item type
                    nextState = ParsingState_ReadingItemType;
                    finishParsing = true;
                }
                // Only other allowed characters are whitespace character and they can be ignored
                else if (XmlValidator::isWhitespace(terminationCharacter))
                {
                    // Check document state
                    if (m_documentState == DocumentState_PrologXmlDeclaration)
                    {
                        // XML declaration is not at the start of the XML string. For a valid XML
                        // document the XML declaration has to be located at the start of the XML
                        // string otherwise it is not allowed to occur in the XML string. Change the
                        // document state accordingly.
                        m_documentState = DocumentState_PrologDocumentType;
                    }
                }
                else
                {
                    // Error, invalid character
                    nextState = ParsingState_Error;
                    finishParsing = true;
                }
                break;
            }

            case XmlItemParser::Result_NeedMoreData:
            {
                // Wait for more data
                finishParsing = true;
                break;
            }

            default:
            {
                // Error
                nextState = ParsingState_Error;
                finishParsing = true;
                break;
            }
        }
    }
    while (!finishParsing);

    return nextState;
}

/**
 * Execute parsing state machine state: Reading item type
 *
 * \retval ParsingState_ReadingItemType     Waiting for more data
 * \retval ParsingState_PiTarget            Found a Processing Instruction item type
 * \retval ParsingState_ElementName         Found an Element item type
 * \retval ParsingState_Comment             Found a Comment item type
 * \retval ParsingState_DocumentTypeName    Found a Document Type item type
 * \retval ParsingState_Error               Error occured
 */
XmlReader::ParsingState XmlReader::executeParsingStateReadingItemType()
{
    ParsingState nextState = ParsingState_ReadingItemType;
    bool finishParsing = false;

    do
    {
        XmlItemParser::Result itemParserResult = m_itemParser.parseItemType();

        switch (itemParserResult)
        {
            case XmlItemParser::Result_Success:
            {
                const XmlItemParser::ItemType itemType = m_itemParser.getItemType();

                switch (itemType)
                {
                    case XmlItemParser::ItemType_ProcessingInstruction:
                    {
                        // Item type: Processing Instruction
                        nextState = ParsingState_PiTarget;
                        break;
                    }

                    case XmlItemParser::ItemType_DocumentType:
                    {
                        // Item type: Document Type
                        nextState = ParsingState_DocumentTypeName;
                        break;
                    }

                    case XmlItemParser::ItemType_Comment:
                    {
                        // Item type: Comment
                        nextState = ParsingState_Comment;
                        break;
                    }

                    case XmlItemParser::ItemType_Element:
                    {
                        // Item type: Element
                        nextState = ParsingState_ElementName;
                        break;
                    }

                    default:
                    {
                        // Error
                        nextState = ParsingState_Error;
                        break;
                    }
                }

                finishParsing = true;
                break;
            }

            case XmlItemParser::Result_NeedMoreData:
            {
                // Wait for more data
                finishParsing = true;
                break;
            }

            default:
            {
                // Error
                nextState = ParsingState_Error;
                finishParsing = true;
                break;
            }
        }
    }
    while (!finishParsing);

    return nextState;
}

/**
 * Execute parsing state machine state: PITarget
 *
 * \retval ParsingState_PiTarget    Waiting for more data
 * \retval ParsingState_PiValue     End of PITarget found
 * \retval ParsingState_PiEnd       End of PITarget with empty value found
 * \retval ParsingState_Error       Error occured
 */
XmlReader::ParsingState XmlReader::executeParsingStatePiTarget()
{
    ParsingState nextState = ParsingState_PiTarget;
    bool finishParsing = false;

    do
    {
        XmlItemParser::Result itemParserResult = m_itemParser.parseName();

        switch (itemParserResult)
        {
            case XmlItemParser::Result_Success:
            {
                // Check termination character
                const uint32_t terminationCharacter = m_itemParser.getTerminationCharacter();

                if (terminationCharacter == (uint32_t)'?')
                {
                    // End of PITarget with empty value found
                    m_name = m_itemParser.getValue();
                    nextState = ParsingState_PiEnd;
                }
                else if (XmlValidator::isWhitespace(terminationCharacter))
                {
                    // End of PITarget found
                    m_name = m_itemParser.getValue();
                    nextState = ParsingState_PiValue;
                }
                else
                {
                    // Error, invalid termination character
                    nextState = ParsingState_Error;
                }

                finishParsing = true;
                break;
            }

            case XmlItemParser::Result_NeedMoreData:
            {
                // Wait for more data
                finishParsing = true;
                break;
            }

            default:
            {
                // Error
                nextState = ParsingState_Error;
                finishParsing = true;
                break;
            }
        }
    }
    while (!finishParsing);

    return nextState;
}

/**
 * Execute parsing state machine state: PI's value
 *
 * \retval ParsingState_PiValue Waiting for more data
 * \retval ParsingState_PiEnd   End of PI'Target's value found
 * \retval ParsingState_Error   Error occured
 */
XmlReader::ParsingState XmlReader::executeParsingStatePiValue()
{
    ParsingState nextState = ParsingState_PiValue;
    bool finishParsing = false;

    do
    {
        XmlItemParser::Result itemParserResult = m_itemParser.parsePiValue();

        switch (itemParserResult)
        {
            case XmlItemParser::Result_Success:
            {
                // Check termination character
                const uint32_t terminationCharacter = m_itemParser.getTerminationCharacter();

                if (terminationCharacter == (uint32_t)'?')
                {
                    // End of PI'Target 's value found
                    m_value = m_itemParser.getValue();
                    nextState = ParsingState_PiEnd;
                }
                else
                {
                    // Error, invalid termination character
                    nextState = ParsingState_Error;
                }

                finishParsing = true;
                break;
            }

            case XmlItemParser::Result_NeedMoreData:
            {
                // Wait for more data
                finishParsing = true;
                break;
            }

            default:
            {
                // Error
                nextState = ParsingState_Error;
                finishParsing = true;
                break;
            }
        }
    }
    while (!finishParsing);

    return nextState;
}

/**
 * Execute parsing state machine state: End of processing instruction
 *
 * \param[out] newResult    New parsing result
 *
 * \retval ParsingState_PiEnd                   Waiting for more data
 * \retval ParsingState_WaitingForStartOfItem   End of PI's value found
 * \retval ParsingState_Error                   Error occured
 *
 * The new parsing result will only be set if end of processing instruction was found and if it is
 * valid. Expected values:
 *  - ParsingResult_XmlDeclaration: XML Declaration was found
 *  - ParsingResult_ProcessingInstruction: XML Processing Instruction was found
 */
XmlReader::ParsingState XmlReader::executeParsingStatePiEnd(XmlReader::ParsingResult *newResult)
{
    ParsingState nextState = ParsingState_PiEnd;
    bool finishParsing = false;

    if (newResult == NULL)
    {
        // Error, null pointer
        nextState = ParsingState_Error;
    }
    else
    {
        do
        {
            XmlItemParser::Result itemParserResult = m_itemParser.parseEndOfItem();

            switch (itemParserResult)
            {
                case XmlItemParser::Result_Success:
                {
                    // Check if processing instruction is the documents XML Declaration
                    nextState = ParsingState_WaitingForStartOfItem;
                    ParsingResult result = ParsingResult_ProcessingInstruction;

                    if (m_name.size() == 3U)
                    {
                        if (((m_name.at(0U) == 'x') || (m_name.at(0U) == 'X')) &&
                            ((m_name.at(1U) == 'm') || (m_name.at(1U) == 'M')) &&
                            ((m_name.at(2U) == 'l') || (m_name.at(2U) == 'L')))
                        {
                            // Parse XML Declaration from the processing instruction's value
                            if (parseXmlDeclaration())
                            {
                                result = ParsingResult_XmlDeclaration;
                            }
                            else
                            {
                                // Error, null pointer
                                nextState = ParsingState_Error;
                            }
                        }
                    }

                    if (nextState != ParsingState_Error)
                    {
                        *newResult = result;
                    }

                    finishParsing = true;
                    break;
                }

                case XmlItemParser::Result_NeedMoreData:
                {
                    // Wait for more data
                    finishParsing = true;
                    break;
                }

                default:
                {
                    // Error
                    nextState = ParsingState_Error;
                    finishParsing = true;
                    break;
                }
            }
        }
        while (!finishParsing);
    }

    return nextState;
}

/**
 * Execute parsing state machine state: Document Type Name
 *
 * \retval ParsingState_DocumentTypeName    Waiting for more data
 * \retval ParsingState_DocumentTypeValue   End of Name found
 * \retval ParsingState_DocumentTypeEnd     End of Name with empty value found
 * \retval ParsingState_Error               Error occured
 */
XmlReader::ParsingState XmlReader::executeParsingStateDocumentTypeName()
{
    ParsingState nextState = ParsingState_DocumentTypeName;

    bool finishParsing = false;

    do
    {
        XmlItemParser::Result itemParserResult = m_itemParser.parseName(XmlItemParser::Option_None);

        switch (itemParserResult)
        {
            case XmlItemParser::Result_Success:
            {
                // Check termination character
                const uint32_t terminationCharacter = m_itemParser.getTerminationCharacter();

                if (terminationCharacter == (uint32_t)'>')
                {
                    // End of Document Type found with empty value found
                    m_name = m_itemParser.getValue();
                    nextState = ParsingState_DocumentTypeEnd;
                }
                else if (XmlValidator::isWhitespace(terminationCharacter))
                {
                    // End of Document Type name found, start parsing Document Type value
                    m_name = m_itemParser.getValue();
                    nextState = ParsingState_DocumentTypeValue;
                }
                else
                {
                    // Error, invalid termination character
                    nextState = ParsingState_Error;
                }

                finishParsing = true;
                break;
            }

            case XmlItemParser::Result_NeedMoreData:
            {
                // Wait for more data
                finishParsing = true;
                break;
            }

            default:
            {
                // Error
                nextState = ParsingState_Error;
                finishParsing = true;
                break;
            }
        }
    }
    while (!finishParsing);

    return nextState;
}

XmlReader::ParsingState XmlReader::executeParsingStateDocumentTypeValue()
{
    ParsingState nextState = ParsingState_DocumentTypeValue;

    // TODO: continue implementation from here!

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
