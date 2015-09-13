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

#include "XmlItemParser.h"
#include "Utf.h"
#include "XmlValidator.h"

using namespace MiniSaxCpp;

static const std::string s_itemTypeComment("!--");
static const std::string s_itemTypeDocumentType("!DOCTYPE");

/**
 * Constructor
 */
XmlItemParser::XmlItemParser(const size_t dataBufferSize)
    : m_xmlDataBuffer(dataBufferSize)
{
    clear();
}

/**
 * Clear internal state and prepare for the first parsing session
 */
void XmlItemParser::clear()
{
    m_xmlDataBuffer.clear();
    m_parsingBuffer.clear();
    m_position = 0U;
    m_option = Option_None;
    m_state = State_Idle;
    m_terminationCharacter = 0U;
    m_itemType = ItemType_None;
    m_value.clear();
}

/**
 * Write data to the data buffer
 *
 * \param data  Data to write
 *
 * \retval true     Success
 * \retval false    Error, data buffer is full
 */
bool XmlItemParser::writeData(const char data)
{
    return m_xmlDataBuffer.write(data);
}

/**
 * Configure parser for next action
 *
 * \param action    Parsing action
 * \param option    Parsing option
 *
 * \retval true     Success
 * \retval false    Error
 */
bool XmlItemParser::configure(const XmlItemParser::Action action,
                               const XmlItemParser::Option option)
{
    bool success = false;

    switch (action)
    {
        case Action_ReadItem:
        {
            if (checkActionReadItem(option))
            {
                m_state = State_WaitForStartOfItem;
                success = true;
            }
            break;
        }

        case Action_ReadName:
        {
            if (checkActionReadName(option))
            {
                m_state = State_ReadName;
                success = true;
            }
            break;
        }

        case Action_ReadPiValue:
        {
            if (checkActionReadPiValue(option))
            {
                m_state = State_ReadPiValue;
                success = true;
            }
            break;
        }

        default:
        {
            break;
        }
    }

    if (success)
    {
        // Save option
        m_option = option;

        // Remove unneeded items from the parsing buffer
        eraseFromParsingBuffer(m_position);

        // Clear output values
        m_terminationCharacter = 0U;
        m_itemType = ItemType_None;
        m_value.clear();
    }

    return success;
}

/**
 * Execute parsing state machine
 *
 * \retval Result_Success       Successfully completed configured action
 * \retval Result_NeedMoreData  More data is needed
 * \retval Result_Error         Error
 */
XmlItemParser::Result XmlItemParser::execute()
{
    Result result = Result_Error;

    // Execute state machine
    bool finishParsing;

    do
    {
        finishParsing = true;
        State nextState = State_Error;

        switch (m_state)
        {
            case State_WaitForStartOfItem:
            {
                // Wait for start of item
                nextState = executeStateWaitForStartOfItem();

                // Check transitions
                switch (nextState)
                {
                    case State_WaitForStartOfItem:
                    {
                        result = Result_NeedMoreData;
                        break;
                    }

                    case State_WhitespaceFound:
                    {
                        result = Result_Success;
                        break;
                    }

                    case State_ReadItemType:
                    {
                        // Execute another cycle
                        finishParsing = false;
                        break;
                    }

                    default:
                    {
                        // Error
                        nextState = State_Error;
                        break;
                    }
                }
                break;
            }

            case State_ReadItemType:
            {
                // Read item type
                nextState = executeStateReadItemType();

                // Check transitions
                switch (nextState)
                {
                    case State_ReadItemType:
                    {
                        result = Result_NeedMoreData;
                        break;
                    }

                    case State_ItemTypeFound:
                    {
                        result = Result_Success;
                        break;
                    }

                    default:
                    {
                        // Error
                        nextState = State_Error;
                        break;
                    }
                }
                break;
            }

            case State_ReadName:
            {
                // Read Name
                nextState = executeStateReadName();

                // Check transitions
                switch (nextState)
                {
                    case State_ReadName:
                    {
                        result = Result_NeedMoreData;
                        break;
                    }

                    case State_NameFound:
                    {
                        result = Result_Success;
                        break;
                    }

                    default:
                    {
                        // Error
                        nextState = State_Error;
                        break;
                    }
                }
                break;
            }

            case State_ReadPiValue:
            {
                // Read Name
                nextState = executeStateReadPiValue();

                // Check transitions
                switch (nextState)
                {
                    case State_ReadPiValue:
                    {
                        result = Result_NeedMoreData;
                        break;
                    }

                    case State_PiValueFound:
                    {
                        result = Result_Success;
                        break;
                    }

                    default:
                    {
                        // Error
                        nextState = State_Error;
                        break;
                    }
                }
                break;
            }

            default:
            {
                break;
            }
        }

        // Update state
        m_state = nextState;
    }
    while (!finishParsing);

    return result;
}

/**
 * Get termination character
 *
 * \return Termination character
 *
 * A "termination character" is a unicode character that was the reason for successful completion of
 * the parsing session.
 */
uint32_t XmlItemParser::getTerminationCharacter()
{
    return m_terminationCharacter;
}

/**
 * Get parsed item type
 *
 * \return Parsed item type
 *
 * \note Item type is only set in the "item type" parser
 */
XmlItemParser::ItemType XmlItemParser::getItemType() const
{
    return m_itemType;
}

/**
 * Get parsed Value string
 *
 * \return Parsed Value string (unicode string)
 *
 * \note The context of this value depends on the item parser that was used
 */
UnicodeString XmlItemParser::getValue() const
{
    return m_value;
}

/**
 * Erase characters from parsing buffer
 *
 * \param size  Number of characters to erase
 *
 * \note Parsing position will be set to 0
 */
void XmlItemParser::eraseFromParsingBuffer(const size_t size)
{
    m_parsingBuffer.erase(0U, size);
    m_position = 0U;
}

/**
 * Read data from the buffer
 *
 * \retval true     Data was read
 * \retval false    Data was not read
 */
bool XmlItemParser::readData()
{
    bool dataRead = false;

    if (!m_xmlDataBuffer.empty())
    {
        // Read next character
        const uint32_t value = m_xmlDataBuffer.read();
        m_parsingBuffer.append(1U, value);
        dataRead = true;
    }

    return dataRead;
}

/**
 * Read data from the buffer if needed
 *
 * \retval true     Data is availabe
 * \retval false    Data is not availabe
 */
bool XmlItemParser::readDataIfNeeded()
{
    bool dataAvailable = true;

    // Check if another character should be read
    if (m_position >= m_parsingBuffer.size())
    {
        dataAvailable = readData();
    }

    return dataAvailable;
}

/**
 * Check if action "Read item" is allowed based on the internal state of the parser
 *
 * \param option    Parsing option (allowed: None, Synchronization, IgnoreLeadingWhitespace)
 *
 * \retval true     Action is allowed
 * \retval false    Action is not allowed
 */
bool XmlItemParser::checkActionReadItem(Option option)
{
    bool allowed = false;

    if ((m_state == State_Idle) ||
        (m_state == State_WhitespaceFound) ||
        (m_state == State_PiValueFound))
    {
        if ((option == Option_None) ||
            (option == Option_Synchronization) ||
            (option == Option_IgnoreLeadingWhitespace))
        {
            allowed = true;
        }
    }

    return allowed;
}

/**
 * Execute state: Wait for start of item
 *
 * \retval State_WaitForStartOfItem Wait for more data
 * \retval State_ReadItemType       Start of item found, read item type
 * \retval State_WhitespaceFound    Whitespace found
 * \retval State_Error              Error
 */
XmlItemParser::State XmlItemParser::executeStateWaitForStartOfItem()
{
    State nextState = State_WaitForStartOfItem;
    bool finishParsing = false;

    do
    {
        // Get next character
        uint32_t unicodeCharacter = 0U;

        if (m_position < m_parsingBuffer.size())
        {
            // Take the next character from the parsing buffer
            unicodeCharacter = m_parsingBuffer.at(m_position);
            m_position++;
        }
        else
        {
            // Try to read the next character from the data buffer
            if (m_xmlDataBuffer.empty())
            {
                // Wait for more data
                finishParsing = true;
            }
            else
            {
                // Read character
                unicodeCharacter = m_xmlDataBuffer.read();
            }
        }

        // Parse value
        if (!finishParsing)
        {
            if (unicodeCharacter == '<')
            {
                // Parsing finished: Start of an XML item found
                eraseFromParsingBuffer(m_position);
                m_terminationCharacter = unicodeCharacter;
                nextState = State_ReadItemType;
                finishParsing = true;
            }
            else
            {
                if (m_option == Option_Synchronization)
                {
                    // On synchronization option ignore all other characters
                }
                else
                {
                    if (XmlValidator::isWhitespace(unicodeCharacter))
                    {
                        if (m_option == Option_IgnoreLeadingWhitespace)
                        {
                            // We are allowed to ignore whitespace characters
                        }
                        else
                        {
                            // Parsing finished: Whitespace character found
                            eraseFromParsingBuffer(m_position);
                            m_terminationCharacter = unicodeCharacter;
                            m_itemType = ItemType_Whitespace;
                            nextState = State_WhitespaceFound;
                            finishParsing = true;
                        }
                    }
                    else
                    {
                        // Error, invalid character read
                        eraseFromParsingBuffer(m_position);
                        m_terminationCharacter = unicodeCharacter;
                        nextState = State_Error;
                        finishParsing = true;
                    }
                }
            }
        }
    }
    while (!finishParsing);

    return nextState;
}

/**
 * Execute state: Read item type
 *
 * \retval State_ReadItemType   Wait for more data
 * \retval State_ItemTypeFound  Item type "processing instruction", "document type", "comment", or
 *                              "element" found
 * \retval State_Error          Error
 *
 * Valid item types:
 * \code{.unparsed}
 * Processing Instruction ::= ?
 * Document Type          ::= !DOCTYPE
 * Comment                ::= !--
 * Element                ::= NameStartChar
 * \endcode
 */
XmlItemParser::State XmlItemParser::executeStateReadItemType()
{
    State nextState = State_Error;
    bool finishParsing = false;

    do
    {
        // Read more data if needed and check if data is available
        if (readDataIfNeeded() == false)
        {
            // No data available, wait for more data
            nextState = State_ReadItemType;
            finishParsing = true;
        }
        else
        {
            // Data available
            if (m_position == 0U)
            {
                // Parse first (unicode) character. Possible item types:
                // - Processing Instruction
                // - Document Type
                // - Comment
                // - Element
                const uint32_t unicodeCharacter = m_parsingBuffer.at(m_position);

                if (unicodeCharacter == (uint32_t)'?')
                {
                    // Item type: Processing Instruction
                    eraseFromParsingBuffer(1U);
                    m_terminationCharacter = unicodeCharacter;
                    m_itemType = ItemType_ProcessingInstruction;
                    nextState = State_ItemTypeFound;
                    finishParsing = true;
                }
                else if (unicodeCharacter == (uint32_t)'!')
                {
                    // Possible types: Document Type and Comment
                    m_position = 1U;
                }
                else
                {
                    if (XmlValidator::isNameStartChar(unicodeCharacter))
                    {
                        // Item type: Element
                        m_terminationCharacter = 0U;
                        m_itemType = ItemType_Element;
                        nextState = State_ItemTypeFound;
                        finishParsing = true;
                    }
                    else
                    {
                        // Error, invalid character
                        m_terminationCharacter = (uint32_t)unicodeCharacter;
                        finishParsing = true;
                    }
                }
            }
            else
            {
                bool checkNextItemType = true;
                const char value = m_parsingBuffer.at(m_position);

                // Check for Comment
                if (m_position < s_itemTypeComment.size())
                {
                    if (value == s_itemTypeComment.at(m_position))
                    {
                        checkNextItemType = false;
                        m_position++;

                        if (m_position == s_itemTypeComment.size())
                        {
                            // Item type: Comment
                            eraseFromParsingBuffer(m_position);
                            m_terminationCharacter = 0U;
                            m_itemType = ItemType_Comment;
                            nextState = State_ItemTypeFound;
                            finishParsing = true;
                        }
                    }
                }

                // Check for Document Type
                if (checkNextItemType)
                {
                    if (m_position < s_itemTypeDocumentType.size())
                    {
                        if (value == s_itemTypeDocumentType.at(m_position))
                        {
                            checkNextItemType = false;
                            m_position++;

                            if (m_position == s_itemTypeDocumentType.size())
                            {
                                // Item type: Document Type
                                eraseFromParsingBuffer(m_position);
                                m_terminationCharacter = 0U;
                                m_itemType = ItemType_DocumentType;
                                nextState = State_ItemTypeFound;
                                finishParsing = true;
                            }
                        }
                    }
                }

                // Check for error
                if (checkNextItemType)
                {
                    // Error, invalid character
                    m_position = 0U;
                    m_terminationCharacter = (uint32_t)value;
                    finishParsing = true;
                }
            }
        }
    }
    while (!finishParsing);

    return nextState;
}

/**
 * Check if action "Read Name" is allowed based on the internal state of the parser
 *
 * \param option    Parsing option (allowed: None)
 *
 * \retval true     Action is allowed
 * \retval false    Action is not allowed
 */
bool XmlItemParser::checkActionReadName(Option option)
{
    bool allowed = false;

    // TODO: add other states?
    if (m_state == State_ItemTypeFound)
    {
        if ((option == Option_None) ||
            (option == Option_IgnoreLeadingWhitespace))
        {
            allowed = true;
        }
    }

    return allowed;
}

/**
 * Execute state: Read Name
 *
 * \retval State_ReadName   Wait for more data
 * \retval State_NameFound  Name found
 * \retval State_Error      Error
 *
 * Valid item types:
 * \code{.unparsed}
 * Name ::= NameStartChar (NameChar)*
 * \endcode
 */
XmlItemParser::State XmlItemParser::executeStateReadName()
{
    State nextState = State_Error;
    bool finishParsing = false;

    do
    {
        // Read more data if needed and check if data is available
        if (readDataIfNeeded() == false)
        {
            // No data available, wait for more data
            nextState = State_ReadName;
            finishParsing = true;
        }
        else
        {
            // Data available
            const uint32_t unicodeCharacter = m_parsingBuffer.at(m_position);

            // Check for start of Name
            if (m_position == 0U)
            {
                if (XmlValidator::isNameStartChar(unicodeCharacter))
                {
                    // Valid NameStartChar found
                    m_position++;
                }
                else
                {
                    bool error = true;

                    if (m_option == Option_IgnoreLeadingWhitespace)
                    {
                        if (XmlValidator::isWhitespace(unicodeCharacter))
                        {
                            // Ignore whitespace
                            eraseFromParsingBuffer(1U);
                            error = false;
                        }
                    }

                    if (error)
                    {
                        // Error, invalid character
                        m_position = 0U;
                        m_terminationCharacter = unicodeCharacter;
                        finishParsing = true;
                    }
                }
            }
            // Check for rest of the Name
            else
            {
                if (XmlValidator::isNameChar(unicodeCharacter))
                {
                    // Valid NameChar found
                    m_position++;
                }
                else
                {
                    // End of Name
                    m_value = m_parsingBuffer.substr(0U, m_position);
                    eraseFromParsingBuffer(m_position);
                    m_terminationCharacter = unicodeCharacter;
                    nextState = State_NameFound;
                    finishParsing = true;
                }
            }
        }
    }
    while (!finishParsing);

    return nextState;
}

/**
 * Check if action "Read processing instruction value" is allowed based on the internal state of the
 * parser
 *
 * \param option    Parsing option (allowed: None)
 *
 * \retval true     Action is allowed
 * \retval false    Action is not allowed
 */
bool XmlItemParser::checkActionReadPiValue(Option option)
{
    bool allowed = false;

    if (m_state == State_NameFound)
    {
        if ((option == Option_None) ||
            (option == Option_IgnoreLeadingWhitespace))
        {
            allowed = true;
        }
    }

    return allowed;
}

/**
 * Execute state: Read processing instruction value
 *
 * \retval State_ReadPiValue    Wait for more data
 * \retval State_PiValueFound   End of processing instruction found
 * \retval State_Error          Error
 *
 * Valid item types:
 * \code{.unparsed}
 * PI value ::= (Char* - (Char* '?>' Char*)))?
 * \endcode
 */
XmlItemParser::State XmlItemParser::executeStateReadPiValue()
{
    State nextState = State_Error;
    bool finishParsing = false;

    do
    {
        // Read more data if needed and check if data is available
        if (readDataIfNeeded() == false)
        {
            // No data available, wait for more data
            nextState = State_ReadPiValue;
            finishParsing = true;
        }
        else
        {
            // Data available
            const uint32_t unicodeCharacter = m_parsingBuffer.at(m_position);

            // Check if character is valid
            if (XmlValidator::isChar(unicodeCharacter))
            {
                if (m_position > 0U)
                {
                    const uint32_t previousCharacter = m_parsingBuffer.at(m_position - 1U);

                    // Check for "?>" sequence
                    if ((previousCharacter == (uint32_t)'?') &&
                        (unicodeCharacter == (uint32_t)'>'))
                    {
                        m_value = m_parsingBuffer.substr(0U, m_position - 1U);
                        eraseFromParsingBuffer(m_position + 1U);
                        m_terminationCharacter = 0U;
                        nextState = State_PiValueFound;
                        finishParsing = true;
                    }
                }

                m_position++;
            }
            else
            {
                // Error, invalid character read
                m_position = 0U;
                m_terminationCharacter = unicodeCharacter;
                finishParsing = true;
            }
        }
    }
    while (!finishParsing);

    return nextState;
}











///**
// * Execute parser: Parse XML Processing Instruction's value item
// *
// * \retval Result_NeedMoreData  More data is needed to complete the parsing session
// * \retval Result_Success       Valid XML Processing Instruction's value found
// * \retval Result_Error         Error occurred (invalid character read)
// */
//XmlItemParser::Result XmlItemParser::parsePiValue()
//{
//    Result result = Result_Error;
//    bool finishParsing = false;

//    do
//    {
//        // Read more data if needed and check if data is available
//        if (readDataIfNeeded() == false)
//        {
//            // No data available, wait for more data
//            result = Result_NeedMoreData;
//            finishParsing = true;
//        }
//        else
//        {
//            // Data available
//            // Get unicode character
//            uint32_t unicodeCharacter = 0U;
//            size_t nextPosition = 0U;
//            Utf::Result utfResult = Utf::unicodeCharacterFromUtf8(m_parsingBuffer,
//                                                                  m_position,
//                                                                  &nextPosition,
//                                                                  &unicodeCharacter);

//            switch (utfResult)
//            {
//                case Utf::Result_Success:
//                {
//                    if (XmlValidator::isChar(unicodeCharacter))
//                    {
//                        if (m_position > 0U)
//                        {
//                            const char previousValue = m_parsingBuffer.at(m_position - 1U);

//                            // Check for "?>" sequence
//                            if ((previousValue == '?') && (unicodeCharacter == (uint32_t)'>'))
//                            {
//                                m_value = m_parsingBuffer.substr(0U, m_position - 1U);
//                                eraseFromParsingBuffer(m_position - 1U);
//                                m_terminationCharacter = (uint32_t)'?';
//                                result = Result_Success;
//                                finishParsing = true;
//                            }
//                        }

//                        m_position++;
//                    }
//                    else
//                    {
//                        // Error, invalid character read
//                        eraseFromParsingBuffer(m_position);
//                        m_terminationCharacter = unicodeCharacter;
//                        finishParsing = true;
//                    }
//                    break;
//                }

//                case Utf::Result_Incomplete:
//                {
//                    // Read next character
//                    if (!readData())
//                    {
//                        // No data available, wait for more data
//                        result = Result_NeedMoreData;
//                        finishParsing = true;
//                    }
//                    break;
//                }

//                default:
//                {
//                    // Error, invalid unicode character
//                    finishParsing = true;
//                    break;
//                }
//            }
//        }
//    }
//    while (!finishParsing);

//    return result;
//}

///**
// * Execute parser: Search for a end of a Processing Instruction
// *
// * \retval Result_NeedMoreData  More data is needed to complete the parsing session
// * \retval Result_Success       End of a Processing Instruction
// * \retval Result_Error         Error occurred (invalid character read)
// *
// * Valid Name string format:
// * \code{.unparsed}
// * End of item ::= >
// * \endcode
// */
//XmlItemParser::Result XmlItemParser::parseEndOfPi()
//{
//    Result result = Result_Error;
//    bool finishParsing = false;

//    do
//    {
//        // Read more data if needed and check if data is available
//        if (readDataIfNeeded() == false)
//        {
//            // No data available, wait for more data
//            result = Result_NeedMoreData;
//            finishParsing = true;
//        }
//        else
//        {
//            // Parse
//            const char value = m_parsingBuffer.at(m_position);

//            if (m_position == 0U)
//            {
//                if (value == '?')
//                {
//                    // First character of "?>" sequence found, check next one
//                    m_position++;
//                }
//                else
//                {
//                    // Error, invalid character
//                    m_terminationCharacter = (uint32_t)value;
//                    finishParsing = true;
//                }
//            }
//            else if (m_position == 1U)
//            {
//                if (value == '>')
//                {
//                    // Second character of "?>" sequence also found
//                    // Parsing finished: End of an XML item found
//                    eraseFromParsingBuffer(2U);
//                    result = Result_Success;
//                }
//                else
//                {
//                    // Error, invalid character
//                }

//                m_position = 0U;
//                m_terminationCharacter = (uint32_t)value;
//                finishParsing = true;
//            }
//            else
//            {
//                // Error, invalid position
//                m_position = 0U;
//                m_terminationCharacter = 0U;
//                finishParsing = true;
//            }
//        }
//    }
//    while (!finishParsing);

//    return result;
//}

///**
// * Execute parser: Parse end of Document Type item
// *
// * \retval Result_NeedMoreData  More data is needed to complete the parsing session
// * \retval Result_Success       End of Document Type found
// * \retval Result_Error         Error occurred (invalid character read)
// *
// * \note This function will ignore all characters between Document Type name and the first '>'
// *       character that does not have a matching '<' preceding it. This is not fully compliant with
// *       the XML standard!
// *
// * \todo Make this fully compliant with the XML standard!
// */
//XmlItemParser::Result XmlItemParser::parseEndOfDocumentType()
//{
//    Result result = Result_Error;
//    bool finishParsing = false;

//    do
//    {
//        // Get character
//        char value = 0U;

//        if (m_position < m_parsingBuffer.size())
//        {
//            value = m_parsingBuffer.at(m_position);
//            m_position++;
//        }
//        else
//        {
//            if (m_xmlDataBuffer.empty())
//            {
//                // No data available, wait for more data
//                clearParsingBuffer();
//                result = Result_NeedMoreData;
//                finishParsing = true;
//            }
//            else
//            {
//                value = m_xmlDataBuffer.read();
//            }
//        }

//        if (!finishParsing)
//        {
//            // Parse
//            switch (value)
//            {
//                case '<':
//                {
//                    if (!m_openTagCharacterFound)
//                    {
//                        m_openTagCharacterFound = true;
//                    }
//                    else
//                    {
//                        // Error, another '<' character without a matching '>' character
//                        finishParsing = true;
//                    }
//                    break;
//                }

//                case '>':
//                {
//                    if (m_openTagCharacterFound)
//                    {
//                        // This is a matching character to the previously found '<' character
//                        m_openTagCharacterFound = false;
//                    }
//                    else
//                    {
//                        // Parsing finished: End of an XML item found
//                        eraseFromParsingBuffer(m_position);
//                        m_terminationCharacter = (uint32_t)value;
//                        result = Result_Success;
//                        finishParsing = true;
//                    }
//                    break;
//                }

//                default:
//                {
//                    // Just ignore the character
//                    break;
//                }
//            }
//        }
//    }
//    while (!finishParsing);

//    return result;
//}

///**
// * Execute parser: Parse Comment item
// *
// * \retval Result_NeedMoreData  More data is needed to complete the parsing session
// * \retval Result_Success       End of Comment found
// * \retval Result_Error         Error occurred (invalid character read)
// */
//XmlItemParser::Result XmlItemParser::parseComment()
//{
//    Result result = Result_Error;
//    bool finishParsing = false;

//    do
//    {
//        // Read more data if needed and check if data is available
//        if (readDataIfNeeded() == false)
//        {
//            // No data available, wait for more data
//            result = Result_NeedMoreData;
//            finishParsing = true;
//        }
//        else
//        {
//            // Data available
//            // Get unicode character
//            uint32_t unicodeCharacter = 0U;
//            size_t nextPosition = 0U;
//            Utf::Result utfResult = Utf::unicodeCharacterFromUtf8(m_parsingBuffer,
//                                                                  m_position,
//                                                                  &nextPosition,
//                                                                  &unicodeCharacter);

//            switch (utfResult)
//            {
//                case Utf::Result_Success:
//                {
//                    // Check for Char
//                    if (XmlValidator::isChar(unicodeCharacter))
//                    {
//                        const size_t currentPosition = m_position;
//                        m_position = nextPosition;

//                        // Check if "-->" sequence
//                        if (currentPosition >= 2U)
//                        {
//                            if ((m_parsingBuffer.at(currentPosition - 2U) == '-') &&
//                                (m_parsingBuffer.at(currentPosition - 1U) == '-'))
//                            {
//                                // Sequence "--" found, check for '>' character
//                                if (m_parsingBuffer.at(currentPosition) == '>')
//                                {
//                                    // Parsing finished: End of Comment found
//                                    m_value = m_parsingBuffer.substr(0U, currentPosition - 2U);
//                                    eraseFromParsingBuffer(currentPosition + 1U);
//                                    result = Result_Success;
//                                }
//                                else
//                                {
//                                    // Error, invalid character
//                                    eraseFromParsingBuffer(currentPosition);
//                                }

//                                m_terminationCharacter = unicodeCharacter;
//                                finishParsing = true;
//                            }
//                        }
//                    }
//                    else
//                    {
//                        // Error, invalid character read
//                        eraseFromParsingBuffer(m_position);
//                        m_terminationCharacter = unicodeCharacter;
//                        finishParsing = true;
//                    }
//                    break;
//                }

//                case Utf::Result_Incomplete:
//                {
//                    // Read next character
//                    if (!readData())
//                    {
//                        // No data available, wait for more data
//                        result = Result_NeedMoreData;
//                        finishParsing = true;
//                    }
//                    break;
//                }

//                default:
//                {
//                    // Error, invalid unicode character
//                    finishParsing = true;
//                    break;
//                }
//            }
//        }
//    }
//    while (!finishParsing);

//    return result;
//}
