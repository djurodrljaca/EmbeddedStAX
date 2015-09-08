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

                // Check for allowed transitions
                if (nextState == ParsingState_ReadingItemType)
                {
                    // Execute another cycle
                    finishParsing = false;
                }
                // Check for invalid transitions
                else
                {
                    if (nextState != ParsingState_WaitingForStartOfItem)
                    {
                        nextState = ParsingState_Error;
                    }
                }
                break;
            }

            case ParsingState_ReadingItemType:
            {
                nextState = executeParsingStateReadingItemType();

                // Check for allowed transitions
                if ((nextState == ParsingState_PiTarget) ||
                    (nextState == ParsingState_ElementName) ||
                    (nextState == ParsingState_Comment) ||
                    (nextState == ParsingState_DocumentTypeName))
                {
                    // Execute another cycle
                    finishParsing = false;
                }
                // Check for invalid transitions
                else
                {
                    if (nextState != ParsingState_ReadingItemType)
                    {
                        nextState = ParsingState_Error;
                    }
                }
                break;
            }

            case ParsingState_PiTarget:
            {
                nextState = executeParsingStatePiTarget();

                // TODO: implement
                break;
            }

            default:
            {
                // Error, invalid state value
                nextState = ParsingState_Error;
                break;
            }
        }

        // Check for error and save parsing state
        if (nextState == ParsingState_Error)
        {
            m_documentState = DocumentState_Error;
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
 * Execute parsing state machine state: Waiting for start of item
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
                        finishParsing = true;
                        break;
                    }

                    case XmlItemParser::ItemType_DocumentType:
                    {
                        // Item type: Document Type
                        nextState = ParsingState_DocumentTypeName;
                        finishParsing = true;
                        break;
                    }

                    case XmlItemParser::ItemType_Comment:
                    {
                        // Item type: Comment
                        nextState = ParsingState_Comment;
                        finishParsing = true;
                        break;
                    }

                    case XmlItemParser::ItemType_Element:
                    {
                        // Item type: Element
                        nextState = ParsingState_ElementName;
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
 * Execute parsing state machine state: Waiting for start of item
 *
 * \retval ParsingState_PiTarget   Waiting for more data
 *
 * \retval ParsingState_Error               Error occured
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

                if ((terminationCharacter == (uint32_t)'?') ||
                    XmlValidator::isWhitespace(terminationCharacter))
                {
                    // End of PITarget found
                    m_name = m_itemParser.getValue();
                    nextState = ParsingState_PiValue;
                    finishParsing = true;
                }
                else
                {
                    // Error, invalid separator
                    nextState = ParsingState_Error;
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
