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
    : m_xmlDataBuffer(dataBufferSize)
{
    clear();
}

/**
 * Clear internal state
 */
void XmlReader::clear()
{
    m_documentState = DocumentState_PrologXmlDeclaration;

    m_xmlDataBuffer.clear();
    clearParsingBuffer();
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
    return m_xmlDataBuffer.write(data);
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
 * Clears parsing buffer
 */
void XmlReader::clearParsingBuffer()
{
    m_parsingBuffer.clear();
    m_parsingBufferPosition = 0U;
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

    // Read data from the XML data buffer and parse it
    bool finishParsing = false;

    while (!m_xmlDataBuffer.empty() &&
           !finishParsing)
    {
        const char value = m_xmlDataBuffer.read();

        if (value == '<')
        {
            // Start of item found, try to read the the item type
            finishParsing = true;
            clearParsingBuffer();
            nextState = ParsingState_ReadingItemType;
        }
        else
        {
            // Only other allowed characters are whitespace character and they can be ignored
            if (XmlValidator::isWhitespace((uint32_t)value))
            {
                // Check document state
                if (m_documentState == DocumentState_PrologXmlDeclaration)
                {
                    // XML declaration is not at the start of the XML string. For a valid XML
                    // document the XML declaration has to be located at the start of the XML string
                    // otherwise it is not allowed to occur in the XML string. Change the document
                    // state accordingly.
                    m_documentState = DocumentState_PrologDocumentType;
                }
            }
            else
            {
                // Error, invalid character
                finishParsing = true;
                nextState = ParsingState_Error;
            }
        }
    }

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

    // Read data from the XML data buffer and parse it
    bool finishParsing = false;

    while (!m_xmlDataBuffer.empty() &&
           !finishParsing)
    {
        const char value = m_xmlDataBuffer.read();
        bool error = true;

        if (m_parsingBufferPosition == 0U)
        {
            // Possible types: Processing Instruction, Document Type, Comment and Element
            if (value == '?')
            {
                // Item type: Processing Instruction
                clearParsingBuffer();
                nextState = ParsingState_PiTarget;
                finishParsing = true;
            }
            else if (value == '!')
            {
                // Possible types: Document Type and Comment
                m_parsingBuffer.append(1U, value);
                m_parsingBufferPosition = 1U;
                error = false;
            }
            else
            {
                // Get unicode character
                uint32_t unicodeCharacter = 0U;
                size_t nextPosition = 0U;
                Utf::Result utfResult = Utf::unicodeCharacterFromUtf8(m_parsingBuffer,
                                                                      m_parsingBufferPosition,
                                                                      &nextPosition,
                                                                      &unicodeCharacter);

                if (utfResult == Utf::Result_Success)
                {
                    if (XmlValidator::isNameStartChar(unicodeCharacter))
                    {
                        // Item type: Element
                        m_parsingBufferPosition = nextPosition;
                        nextState = ParsingState_ElementName;
                        finishParsing = true;
                    }
                }
                else if (utfResult == Utf::Result_Incomplete)
                {
                    // Wait for more data to get the complete multibyte unicode character
                    error = false;
                }
                else
                {
                    // Error
                }
            }
        }
        else
        {
            // Possible types: document type and comment
            m_parsingBuffer.append(1U, value);

            const size_t size = m_parsingBuffer.size();
            m_parsingBufferPosition = size;
            bool searchForItemType = true;

            // Check for Comment
            if (size <= 3U)
            {
                if (m_parsingBuffer.compare(0U, size, "!--", size) == 0)
                {
                    if (size == 3U)
                    {
                        // Item type: Comment
                        clearParsingBuffer();
                        nextState = ParsingState_Comment;
                        finishParsing = true;
                    }
                    else
                    {
                        // More data is needed
                        error = false;
                    }

                    searchForItemType = false;
                }
            }

            // Check for Document Type
            if ((size <= 8U) && searchForItemType)
            {
                if (m_parsingBuffer.compare(0U, size, "!DOCTYPE", size) == 0)
                {
                    if (size == 8U)
                    {
                        // Item type: Document Type
                        clearParsingBuffer();
                        nextState = ParsingState_DocumentTypeName;
                        finishParsing = true;
                    }
                    else
                    {
                        // More data is needed
                        error = false;
                    }

                    searchForItemType = false;
                }
            }
        }

        // Check for error
        if (error)
        {
            // Error, unexpected character
            nextState = ParsingState_Error;
            break;
        }
    }

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

    // Read data from the XML data buffer and parse it
    bool finishParsing = false;

    while (!m_xmlDataBuffer.empty() &&
           !finishParsing)
    {
        const char value = m_xmlDataBuffer.read();
        bool error = true;

        if (m_parsingBufferPosition == 0U)
        {
            // TODO: implement
        }
    }

    return nextState;
}
