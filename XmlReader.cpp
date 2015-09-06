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
    m_documentState = DocumentState_Prolog;

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
    ParsingResult result = ParsingResult_Error;

    // Execute parsing state machine
    bool cycleFinished;

    do
    {
        cycleFinished = true;
        ParsingState nextState = m_parsingState;

        switch (m_parsingState)
        {
            case ParsingState_WaitingForStartOfItem:
            {
                nextState = executeParsingStateWaitingForStartOfItem();
                break;
            }

            case ParsingState_ReadingItemType:
            {
                nextState = executeParsingStateReadingItemType();
                break;
            }

            default:
            {
                // Error, invalid state value
                nextState = ParsingState_Error;
                m_parsingState = ParsingState_Error;
                m_documentState = DocumentState_Error;
                break;
            }
        }

        // Check if parsing state machine cycle should continue or finish
        switch (nextState)
        {
            case ParsingState_ReadingItemType:
            {
                if (m_parsingState == ParsingState_WaitingForStartOfItem)
                {
                    // Execute another cycle
                    cycleFinished = false;
                }
                break;
            }

            default:
            {
                // Cycle should finish
                break;
            }
        }

        // Save parsing state
        m_parsingState = nextState;
    }
    while(!cycleFinished);

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
    while (!m_xmlDataBuffer.empty())
    {
        const char data = m_xmlDataBuffer.read();

        if (data == '<')
        {
            // Start of item found, try to read the the item type
            clearParsingBuffer();
            nextState = ParsingState_ReadingItemType;
            break;
        }
    }

    return nextState;
}

/**
 * Execute parsing state machine state: Waiting for start of item
 *
 * \retval ParsingState_ReadingItemType Waiting for more data
 */
XmlReader::ParsingState XmlReader::executeParsingStateReadingItemType()
{
    ParsingState nextState = ParsingState_WaitingForStartOfItem;

    // Read data from the XML data buffer and parse it
    while (!m_xmlDataBuffer.empty())
    {
        // Read data and try to convert the parsed data to unicode character
        const char data = m_xmlDataBuffer.read();
        m_parsingBuffer.append(1U, data);

        uint32_t unicodeCharacter = 0U;
        Utf::Result utfResult = Utf::unicodeCharacterFromUtf8(m_parsingBuffer,
                                                              m_parsingBufferPosition,
                                                              &m_parsingBufferPosition,
                                                              &unicodeCharacter);

        if (utfResult == Utf::Result_Success)
        {
            // Try to determine the item type
            if (unicodeCharacter == (uint32_t)'?')
            {
                // Item type: Processing Instruction
                clearParsingBuffer();
                nextState = ParsingState_PiReadingPiTarget;
                break;
            }
            else
            {
                // TODO: whitespace
                // TODO: document type: "!DOCTYPE"
                // TODO: comment: "!--"
                // TODO: element start tag: "Name"
                // TODO: ?
            }
        }
        else if (utfResult == Utf::Result_Incomplete)
        {
            // Wait for more data
        }
        else
        {
            // Error
            nextState = ParsingState_Error;
            break;
        }
    }

    return nextState;
}
