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
static const std::string s_itemTypeCData("![CDATA[");

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
    m_quotationMark = Common::QuotationMark_None;
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
                m_state = State_WaitingForStartOfItem;
                success = true;
            }
            break;
        }

        case Action_ReadName:
        {
            if (checkActionReadName(option))
            {
                m_state = State_ReadingName;
                success = true;
            }
            break;
        }

        case Action_ReadPiValue:
        {
            if (checkActionReadPiValue(option))
            {
                m_state = State_ReadingPiValue;
                success = true;
            }
            break;
        }

        case Action_ReadDocumentTypeValue:
        {
            if (checkActionReadDocumentTypeValue(option))
            {
                m_state = State_ReadingDocumentTypeValue;
                success = true;
            }
            break;
        }

        case Action_ReadCommentText:
        {
            if (checkActionReadCommentText(option))
            {
                m_state = State_ReadingCommentText;
                success = true;
            }
            break;
        }

        case Action_ReadAttributeName:
        {
            if (checkActionReadAttributeName(option))
            {
                m_state = State_ReadingAttributeName;
                success = true;
            }
            break;
        }

        case Action_ReadAttributeValue:
        {
            if (checkActionReadAttributeValue(option))
            {
                m_state = State_ReadingAttributeValueEqual;
                success = true;
            }
            break;
        }

        case Action_ReadTextNode:
        {
            if (checkActionReadTextNode(option))
            {
                m_state = State_ReadingTextNode;
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
            case State_WaitingForStartOfItem:
            {
                // Wait for start of item
                nextState = executeStateWaitingForStartOfItem();

                // Check transitions
                switch (nextState)
                {
                    case State_WaitingForStartOfItem:
                    {
                        result = Result_NeedMoreData;
                        break;
                    }

                    case State_WhitespaceRead:
                    {
                        result = Result_Success;
                        break;
                    }

                    case State_ReadingItemType:
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

            case State_ReadingItemType:
            {
                // Read item type
                nextState = executeStateReadingItemType();

                // Check transitions
                switch (nextState)
                {
                    case State_ReadingItemType:
                    {
                        result = Result_NeedMoreData;
                        break;
                    }

                    case State_ItemTypeRead:
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

            case State_ReadingName:
            {
                // Read Name
                nextState = executeStateReadingName();

                // Check transitions
                switch (nextState)
                {
                    case State_ReadingName:
                    {
                        result = Result_NeedMoreData;
                        break;
                    }

                    case State_NameRead:
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

            case State_ReadingPiValue:
            {
                // Read processing instruction value
                nextState = executeStateReadingPiValue();

                // Check transitions
                switch (nextState)
                {
                    case State_ReadingPiValue:
                    {
                        result = Result_NeedMoreData;
                        break;
                    }

                    case State_PiValueRead:
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

            case State_ReadingDocumentTypeValue:
            {
                // Read document type value
                nextState = executeStateReadingDocumentTypeValue();

                // Check transitions
                switch (nextState)
                {
                    case State_ReadingDocumentTypeValue:
                    {
                        result = Result_NeedMoreData;
                        break;
                    }

                    case State_DocumentTypeValueRead:
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

            case State_ReadingCommentText:
            {
                // Read comment text
                nextState = executeStateReadingCommentText();

                // Check transitions
                switch (nextState)
                {
                    case State_ReadingCommentText:
                    {
                        result = Result_NeedMoreData;
                        break;
                    }

                    case State_CommentTextRead:
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

            case State_ReadingAttributeName:
            {
                // Read Name
                nextState = executeStateReadingAttributeName();

                // Check transitions
                switch (nextState)
                {
                    case State_ReadingAttributeName:
                    {
                        result = Result_NeedMoreData;
                        break;
                    }

                    case State_AttributeNameRead:
                    {
                        result = Result_Success;
                        break;
                    }

                    case State_ReadingElementStartOfContent:
                    {
                        // Execute another cycle
                        finishParsing = false;
                        break;
                    }

                    case State_ReadingElementEndEmpty:
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

            case State_ReadingAttributeValueEqual:
            {
                // Read attribute value: equal sign
                nextState = executeStateReadingAttributeValueEqual();

                // Check transitions
                switch (nextState)
                {
                    case State_ReadingAttributeValueEqual:
                    {
                        result = Result_NeedMoreData;
                        break;
                    }

                    case State_ReadingAttributeValueQuote:
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

            case State_ReadingAttributeValueQuote:
            {
                // Read attribute value: quote
                nextState = executeStateReadingAttributeValueQuote();

                // Check transitions
                switch (nextState)
                {
                    case State_ReadingAttributeValueQuote:
                    {
                        result = Result_NeedMoreData;
                        break;
                    }

                    case State_ReadingAttributeValueContent:
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

            case State_ReadingAttributeValueContent:
            {
                // Read attribute value content
                nextState = executeStateReadingAttributeValueContent();

                // Check transitions
                switch (nextState)
                {
                    case State_ReadingAttributeValueContent:
                    {
                        result = Result_NeedMoreData;
                        break;
                    }

                    case State_AttributeValueRead:
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

            case State_ReadingElementStartOfContent:
            {
                // Read start of element's content
                nextState = executeStateReadingElementStartOfContent();

                // Check transitions
                switch (nextState)
                {
                    case State_ReadingElementStartOfContent:
                    {
                        result = Result_NeedMoreData;
                        break;
                    }

                    case State_ElementStartOfContentRead:
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

            case State_ReadingTextNode:
            {
                // Read text node
                nextState = executeStateReadingTextNode();

                // Check transitions
                switch (nextState)
                {
                    case State_ReadingTextNode:
                    {
                        result = Result_NeedMoreData;
                        break;
                    }

                    case State_TextNodeRead:
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

    // TODO: add other states?
    if ((m_state == State_Idle) ||
        (m_state == State_WhitespaceRead) ||
        (m_state == State_PiValueRead) ||
        (m_state == State_DocumentTypeValueRead) ||
        (m_state == State_CommentTextRead) ||
        (m_state == State_ElementStartOfContentRead) ||
        (m_state == State_ElementEndEmptyRead) ||
        (m_state == State_TextNodeRead))
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
 * Execute state: Waiting for start of item
 *
 * \retval State_WaitingForStartOfItem  Wait for more data
 * \retval State_ReadingItemType        Start of item read, read item type
 * \retval State_WhitespaceRead         Whitespace read
 * \retval State_Error                  Error
 *
 * Format:
 * \code{.unparsed}
 * Start of item ::= '<' | S
 * \endcode
 */
XmlItemParser::State XmlItemParser::executeStateWaitingForStartOfItem()
{
    State nextState = State_Error;
    bool finishParsing = false;

    do
    {
        // Read more data if needed and check if data is available
        if (readDataIfNeeded() == false)
        {
            // No data available, wait for more data
            nextState = State_WaitingForStartOfItem;
            finishParsing = true;
        }
        else
        {
            // Data available
            const uint32_t unicodeCharacter = m_parsingBuffer.at(0U);

            if (unicodeCharacter == '<')
            {
                // Parsing finished: Start of an XML item found
                eraseFromParsingBuffer(1U);
                m_terminationCharacter = unicodeCharacter;
                nextState = State_ReadingItemType;
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
                            eraseFromParsingBuffer(1U);
                        }
                        else
                        {
                            // Parsing finished: Whitespace character found
                            eraseFromParsingBuffer(1U);
                            m_terminationCharacter = unicodeCharacter;
                            m_itemType = ItemType_Whitespace;
                            nextState = State_WhitespaceRead;
                            finishParsing = true;
                        }
                    }
                    else
                    {
                        // Error, invalid character read
                        m_terminationCharacter = unicodeCharacter;
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
 * Execute state: Reading item type
 *
 * \retval State_ReadingItemType    Wait for more data
 * \retval State_ItemTypeRead       Item type "processing instruction", "document type", "comment",
 *                                  or "element" found
 * \retval State_Error              Error
 *
 * Format of valid item types:
 * \code{.unparsed}
 * Processing Instruction ::= '?'
 * Document Type          ::= '!DOCTYPE'
 * Comment                ::= '!--'
 * CData                  ::= '![CDATA['
 * Element                ::= NameStartChar
 * \endcode
 */
XmlItemParser::State XmlItemParser::executeStateReadingItemType()
{
    State nextState = State_Error;
    bool finishParsing = false;

    do
    {
        // Read more data if needed and check if data is available
        if (readDataIfNeeded() == false)
        {
            // No data available, wait for more data
            nextState = State_ReadingItemType;
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
                // - CData
                // - Element
                const uint32_t unicodeCharacter = m_parsingBuffer.at(m_position);

                if (unicodeCharacter == (uint32_t)'?')
                {
                    // Item type: Processing Instruction
                    eraseFromParsingBuffer(1U);
                    m_terminationCharacter = unicodeCharacter;
                    m_itemType = ItemType_ProcessingInstruction;
                    nextState = State_ItemTypeRead;
                    finishParsing = true;
                }
                else if (unicodeCharacter == (uint32_t)'!')
                {
                    // Possible types: Document Type, Comment and CData
                    m_position = 1U;
                }
                else
                {
                    if (XmlValidator::isNameStartChar(unicodeCharacter))
                    {
                        // Item type: Start of Element
                        m_terminationCharacter = 0U;
                        m_itemType = ItemType_StartOfElement;
                        nextState = State_ItemTypeRead;
                        finishParsing = true;
                    }
                    else
                    {
                        // Error, invalid character
                        m_terminationCharacter = unicodeCharacter;
                        finishParsing = true;
                    }
                }
            }
            else
            {
                // TODO: save the exact type of most likely candidate for a item to a local variable

                bool checkNextItemType = true;
                const uint32_t unicodeCharacter = m_parsingBuffer.at(m_position);

                // Check for Comment
                if (m_position < s_itemTypeComment.size())
                {
                    if (unicodeCharacter == (uint32_t)s_itemTypeComment.at(m_position))
                    {
                        checkNextItemType = false;
                        m_position++;

                        if (m_position == s_itemTypeComment.size())
                        {
                            // Item type: Comment
                            eraseFromParsingBuffer(m_position);
                            m_terminationCharacter = 0U;
                            m_itemType = ItemType_Comment;
                            nextState = State_ItemTypeRead;
                            finishParsing = true;
                        }
                    }
                }

                // Check for Document Type
                if (checkNextItemType)
                {
                    const size_t size = s_itemTypeDocumentType.size();

                    if (m_position < size)
                    {
                        if (unicodeCharacter == (uint32_t)s_itemTypeDocumentType.at(m_position))
                        {
                            checkNextItemType = false;
                            m_position++;
                        }
                    }
                    else
                    {
                        checkNextItemType = false;

                        if ((m_position == size) &&
                            (XmlValidator::isWhitespace(unicodeCharacter)))
                        {
                            // Item type: Document Type
                            eraseFromParsingBuffer(m_position);
                            m_terminationCharacter = 0U;
                            m_itemType = ItemType_DocumentType;
                            nextState = State_ItemTypeRead;
                        }
                        else
                        {
                            // Error, invalid character
                            m_position = 0U;
                            m_terminationCharacter = unicodeCharacter;
                        }

                        finishParsing = true;
                    }
                }

                // Check for CData
                if (checkNextItemType)
                {
                    if (m_position < s_itemTypeCData.size())
                    {
                        if (unicodeCharacter == (uint32_t)s_itemTypeCData.at(m_position))
                        {
                            checkNextItemType = false;
                            m_position++;

                            if (m_position == s_itemTypeCData.size())
                            {
                                // Item type: CData
                                eraseFromParsingBuffer(m_position);
                                m_terminationCharacter = 0U;
                                m_itemType = ItemType_CData;
                                nextState = State_ItemTypeRead;
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
                    m_terminationCharacter = unicodeCharacter;
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
 * \param option    Parsing option (allowed: None, IgnoreLeadingWhitespace)
 *
 * \retval true     Action is allowed
 * \retval false    Action is not allowed
 */
bool XmlItemParser::checkActionReadName(Option option)
{
    bool allowed = false;

    if (m_state == State_ItemTypeRead)
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
 * Execute state: Reading Name
 *
 * \retval State_ReadingName    Wait for more data
 * \retval State_NameRead       Name read
 * \retval State_Error          Error
 *
 * Format:
 * \code{.unparsed}
 * Name ::= NameStartChar (NameChar)*
 * \endcode
 */
XmlItemParser::State XmlItemParser::executeStateReadingName()
{
    State nextState = State_Error;
    bool finishParsing = false;

    do
    {
        // Read more data if needed and check if data is available
        if (readDataIfNeeded() == false)
        {
            // No data available, wait for more data
            nextState = State_ReadingName;
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
                    nextState = State_NameRead;
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
 * \param option    Parsing option (allowed: None, IgnoreLeadingWhitespace)
 *
 * \retval true     Action is allowed
 * \retval false    Action is not allowed
 */
bool XmlItemParser::checkActionReadPiValue(Option option)
{
    bool allowed = false;

    if (m_state == State_NameRead)
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
 * Execute state: Reading processing instruction value
 *
 * \retval State_ReadingPiValue Wait for more data
 * \retval State_PiValueRead    Processing instruction read
 * \retval State_Error          Error
 *
 * Format:
 * \code{.unparsed}
 * PI value ::= (Char* - (Char* '?>' Char*)))?
 * \endcode
 */
XmlItemParser::State XmlItemParser::executeStateReadingPiValue()
{
    State nextState = State_Error;
    bool finishParsing = false;

    do
    {
        // Read more data if needed and check if data is available
        if (readDataIfNeeded() == false)
        {
            // No data available, wait for more data
            nextState = State_ReadingPiValue;
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
                        nextState = State_PiValueRead;
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

/**
 * Check if action "Read document type value" is allowed based on the internal state of the parser
 *
 * \param option    Parsing option (allowed: None, IgnoreLeadingWhitespace)
 *
 * \retval true     Action is allowed
 * \retval false    Action is not allowed
 */
bool XmlItemParser::checkActionReadDocumentTypeValue(Option option)
{
    bool allowed = false;

    if (m_state == State_NameRead)
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
 * Execute state: Reading document type value
 *
 * \retval State_ReadingDocumentTypeValue   Wait for more data
 * \retval State_DocumentTypeValueRead      Document type value read
 * \retval State_Error                      Error
 *
 * Format:
 * \todo This should be improved, currently it finishes on first '>' character
 */
XmlItemParser::State XmlItemParser::executeStateReadingDocumentTypeValue()
{
    State nextState = State_Error;
    bool finishParsing = false;

    do
    {
        // Read more data if needed and check if data is available
        if (readDataIfNeeded() == false)
        {
            // No data available, wait for more data
            nextState = State_ReadingDocumentTypeValue;
            finishParsing = true;
        }
        else
        {
            // Data available
            const uint32_t unicodeCharacter = m_parsingBuffer.at(m_position);

            // Check if character is valid
            if (unicodeCharacter == (uint32_t)'>')
            {
                m_value = m_parsingBuffer.substr(0U, m_position);
                eraseFromParsingBuffer(m_position + 1U);
                m_terminationCharacter = 0U;
                nextState = State_DocumentTypeValueRead;
                finishParsing = true;
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

/**
 * Check if action "Read comment text" is allowed based on the internal state of the parser
 *
 * \param option    Parsing option (allowed: None)
 *
 * \retval true     Action is allowed
 * \retval false    Action is not allowed
 */
bool XmlItemParser::checkActionReadCommentText(Option option)
{
    bool allowed = false;

    if (m_state == State_ItemTypeRead)
    {
        if (option == Option_None)
        {
            allowed = true;
        }
    }

    return allowed;
}

/**
 * Execute state: Reading comment text
 *
 * \retval State_ReadingCommentText Wait for more data
 * \retval State_CommentTextRead    Comment text read
 * \retval State_Error              Error
 *
 * Format:
 * \code{.unparsed}
 * Comment text ::= ((Char - '-') | ('-' (Char - '-')))*
 * \endcode
 */
XmlItemParser::State XmlItemParser::executeStateReadingCommentText()
{
    State nextState = State_Error;
    bool finishParsing = false;

    do
    {
        // Read more data if needed and check if data is available
        if (readDataIfNeeded() == false)
        {
            // No data available, wait for more data
            nextState = State_ReadingCommentText;
            finishParsing = true;
        }
        else
        {
            // Data available
            const uint32_t unicodeCharacter = m_parsingBuffer.at(m_position);

            // Check if character is valid
            if (XmlValidator::isChar(unicodeCharacter))
            {
                m_position++;

                // Check for "-->" sequence
                if (m_position >= 3U)
                {
                    if ((m_parsingBuffer.at(m_position - 3U) == (uint32_t)'-') &&
                        (m_parsingBuffer.at(m_position - 2U) == (uint32_t)'-'))
                    {
                        // Sequence "--" found, check for '>' character
                        if (unicodeCharacter == (uint32_t)'>')
                        {
                            m_value = m_parsingBuffer.substr(0U, m_position - 3U);
                            eraseFromParsingBuffer(m_position);
                            m_terminationCharacter = 0U;
                            nextState = State_CommentTextRead;
                        }
                        else
                        {
                            // Error, invalid character
                            m_position = 0U;
                            m_terminationCharacter = unicodeCharacter;
                        }

                        finishParsing = true;
                    }
                }
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

/**
 * Check if action "Read Attribute Name" is allowed based on the internal state of the parser
 *
 * \param option    Parsing option (allowed: None, IgnoreLeadingWhitespace)
 *
 * \retval true     Action is allowed
 * \retval false    Action is not allowed
 */
bool XmlItemParser::checkActionReadAttributeName(Option option)
{
    bool allowed = false;

    if ((m_state == State_NameRead) ||
        (m_state == State_AttributeValueRead))
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
 * Execute state: Reading Attribute Name
 *
 * \retval State_ReadingAttributeName           Wait for more data
 * \retval State_NameAttributeRead              Attribute Name read
 * \retval State_ReadingElementStartOfContent   Start of element content read
 * \retval State_ReadingElementEndEmpty         End of empty element read
 * \retval State_Error                          Error
 *
 * Format:
 * \code{.unparsed}
 * Name ::= NameStartChar (NameChar)*
 * \endcode
 */
XmlItemParser::State XmlItemParser::executeStateReadingAttributeName()
{
    State nextState = State_Error;
    bool finishParsing = false;

    do
    {
        // Read more data if needed and check if data is available
        if (readDataIfNeeded() == false)
        {
            // No data available, wait for more data
            nextState = State_ReadingAttributeName;
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
                else if (unicodeCharacter == (uint32_t)'>')
                {
                    // Start of element content found
                    m_terminationCharacter = 0U;
                    nextState = State_ReadingElementStartOfContent;
                    finishParsing = true;
                }
                else if (unicodeCharacter == (uint32_t)'/')
                {
                    // Start of element content found
                    m_terminationCharacter = 0U;
                    nextState = State_ReadingElementEndEmpty;
                    finishParsing = true;
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
                    m_itemType = ItemType_ElementAttribute;
                    nextState = State_AttributeNameRead;
                    finishParsing = true;
                }
            }
        }
    }
    while (!finishParsing);

    return nextState;
}

/**
 * Execute state: Reading element start of content
 *
 * \retval State_ReadingElementStartOfContent   Wait for more data
 * \retval State_ElementStartOfContentRead      Start of element's content read
 * \retval State_Error                          Error
 *
 * Format:
 * \code{.unparsed}
 * Element start of content ::= '>'
 * \endcode
 */
XmlItemParser::State XmlItemParser::executeStateReadingElementStartOfContent()
{
    State nextState = State_Error;

    // Read more data if needed and check if data is available
    if (readDataIfNeeded() == false)
    {
        // No data available, wait for more data
        nextState = State_ReadingElementStartOfContent;
    }
    else
    {
        // Data available
        const uint32_t unicodeCharacter = m_parsingBuffer.at(m_position);

        // Check if character is valid
        if (unicodeCharacter == (uint32_t)'>')
        {
            m_value.clear();
            eraseFromParsingBuffer(m_position + 1U);
            m_terminationCharacter = 0U;
            m_itemType = ItemType_StartOfElementContent;
            nextState = State_ElementStartOfContentRead;
        }
        else
        {
            // Error, invalid character read
            m_position = 0U;
            m_terminationCharacter = unicodeCharacter;
        }
    }

    return nextState;
}

/**
 * Execute state: Reading end of empty element
 *
 * \retval State_ReadingElementEndEmpty Wait for more data
 * \retval State_ElementEndEmptyRead    Document type value read
 * \retval State_Error                  Error
 *
 * Format:
 * \code{.unparsed}
 * End of empty element ::= '/>'
 * \endcode
 */
XmlItemParser::State XmlItemParser::executeStateReadingElementEndEmpty()
{
    State nextState = State_Error;
    bool finishParsing = false;

    do
    {
        // Read more data if needed and check if data is available
        if (readDataIfNeeded() == false)
        {
            // No data available, wait for more data
            nextState = State_ReadingElementEndEmpty;
            finishParsing = true;
        }
        else
        {
            // Data available
            bool success = false;
            const uint32_t unicodeCharacter = m_parsingBuffer.at(m_position);

            if (m_position == 0U)
            {
                // Check if character is valid
                if (unicodeCharacter == (uint32_t)'/')
                {
                    // Check next character
                    m_position = 1U;
                    success = true;
                }
                else
                {
                    // Error, invalid character read
                }
            }
            else if (m_position == 1U)
            {
                // Check if character is valid
                if (unicodeCharacter == (uint32_t)'>')
                {
                    eraseFromParsingBuffer(m_position + 1U);
                    m_terminationCharacter = 0U;
                    m_itemType = ItemType_StartOfEmptyElement;
                    nextState = State_ElementEndEmptyRead;
                    finishParsing = true;
                }
                else
                {
                    // Error, invalid character read
                }
            }
            else
            {
                // Error, invalid position
            }

            if (!success)
            {
                // Error
                m_position = 0U;
                m_terminationCharacter = unicodeCharacter;
                finishParsing = true;
            }
        }
    }
    while (!finishParsing);

    return nextState;
}

/**
 * Check if action "Read attribute value" is allowed based on the internal state of the parser
 *
 * \param option    Parsing option (allowed: None)
 *
 * \retval true     Action is allowed
 * \retval false    Action is not allowed
 */
bool XmlItemParser::checkActionReadAttributeValue(Option option)
{
    bool allowed = false;

    if (m_state == State_AttributeNameRead)
    {
        if (option == Option_None)
        {
            allowed = true;
        }
    }

    return allowed;
}

/**
 * Execute state: Reading attribute value: equal sign
 *
 * \retval State_ReadingAttributeValueEqual Wait for more data
 * \retval State_ReadingAttributeValueQuote Equal sign found
 * \retval State_Error                      Error
 *
 * Format:
 * \code{.unparsed}
 * Start of attribute value ::= S? '='
 * \endcode
 */
XmlItemParser::State XmlItemParser::executeStateReadingAttributeValueEqual()
{
    State nextState = State_Error;
    bool finishParsing = false;

    do
    {
        // Read more data if needed and check if data is available
        if (readDataIfNeeded() == false)
        {
            // No data available, wait for more data
            nextState = State_ReadingAttributeValueEqual;
            finishParsing = true;
        }
        else
        {
            // Data available
            const uint32_t unicodeCharacter = m_parsingBuffer.at(0U);

            if (XmlValidator::isWhitespace(unicodeCharacter))
            {
                // Ignore leading whitespace
                eraseFromParsingBuffer(1U);
            }
            else if (unicodeCharacter == (uint32_t)'=')
            {
                // Parsing finished: Equal sign found
                eraseFromParsingBuffer(1U);
                m_terminationCharacter = unicodeCharacter;
                nextState = State_ReadingAttributeValueQuote;
                finishParsing = true;
            }
            else
            {
                // Error, invalid character read
                m_terminationCharacter = unicodeCharacter;
                finishParsing = true;
            }
        }
    }
    while (!finishParsing);

    return nextState;
}

/**
 * Execute state: Reading attribute value: quote
 *
 * \retval State_ReadingAttributeValueQuote     Wait for more data
 * \retval State_ReadingAttributeValueContent   Equal sign found
 * \retval State_Error                          Error
 *
 * Format:
 * \code{.unparsed}
 * Start of attribute value ::= S? ('"' | "'")
 * \endcode
 */
XmlItemParser::State XmlItemParser::executeStateReadingAttributeValueQuote()
{
    State nextState = State_Error;
    bool finishParsing = false;

    do
    {
        // Read more data if needed and check if data is available
        if (readDataIfNeeded() == false)
        {
            // No data available, wait for more data
            nextState = State_ReadingAttributeValueQuote;
            finishParsing = true;
        }
        else
        {
            // Data available
            const uint32_t unicodeCharacter = m_parsingBuffer.at(0U);

            if (XmlValidator::isWhitespace(unicodeCharacter))
            {
                // Ignore leading whitespace
                eraseFromParsingBuffer(1U);
            }
            else if (unicodeCharacter == (uint32_t)'"')
            {
                // Parsing finished: Quote found
                m_quotationMark = Common::QuotationMark_Quote;
                eraseFromParsingBuffer(1U);
                m_terminationCharacter = 0U;
                nextState = State_ReadingAttributeValueContent;
                finishParsing = true;
            }
            else if (unicodeCharacter == (uint32_t)'\'')
            {
                // Parsing finished: Apostrophe found
                m_quotationMark = Common::QuotationMark_Apostrophe;
                eraseFromParsingBuffer(1U);
                m_terminationCharacter = 0U;
                nextState = State_ReadingAttributeValueContent;
                finishParsing = true;
            }
            else
            {
                // Error, invalid character read
                m_terminationCharacter = unicodeCharacter;
                finishParsing = true;
            }
        }
    }
    while (!finishParsing);

    return nextState;
}

/**
 * Execute state: Reading processing instruction value
 *
 * \retval State_ReadingAttributeValueContent   Wait for more data
 * \retval State_AttributeValueRead             Attribute value read
 * \retval State_Error                          Error
 *
 * Format:
 * \code{.unparsed}
 * AttValue ::= '"' ([^<&"] | Reference)* '"'
 *            | "'" ([^<&'] | Reference)* "'"
 *
 * Reference ::= EntityRef | CharRef
 * EntityRef ::= '&' Name ';'
 * CharRef   ::= '&#' [0-9]+ ';'
 *             | '&#x' [0-9a-fA-F]+ ';'
 * \endcode
 *
 * \todo Check for all possible values! An example can also be found in the XmlValidatior.
 */
XmlItemParser::State XmlItemParser::executeStateReadingAttributeValueContent()
{
    State nextState = State_Error;
    bool finishParsing = false;

    do
    {
        // Read more data if needed and check if data is available
        if (readDataIfNeeded() == false)
        {
            // No data available, wait for more data
            nextState = State_ReadingAttributeValueContent;
            finishParsing = true;
        }
        else
        {
            // Data available
            const uint32_t unicodeCharacter = m_parsingBuffer.at(m_position);

            // Check if character is valid
            if (unicodeCharacter == (uint32_t)'<')
            {
                // Error, invalid character read
                m_position = 0U;
                m_terminationCharacter = unicodeCharacter;
                finishParsing = true;
            }
            else if (((m_quotationMark == Common::QuotationMark_Quote) &&
                      (unicodeCharacter == (uint32_t)'"')) ||
                     ((m_quotationMark == Common::QuotationMark_Apostrophe) &&
                      (unicodeCharacter == (uint32_t)'\'')))
            {
                // End of attribute value found
                m_value = m_parsingBuffer.substr(0U, m_position);
                eraseFromParsingBuffer(m_position + 1U);
                m_terminationCharacter = 0U;
                nextState = State_AttributeValueRead;
                finishParsing = true;
            }
            else
            {
                // Valid attribute value character
                m_position++;
            }
        }
    }
    while (!finishParsing);

    return nextState;
}

/**
 * Check if action "Read text node" is allowed based on the internal state of the parser
 *
 * \param option    Parsing option (allowed: None)
 *
 * \retval true     Action is allowed
 * \retval false    Action is not allowed
 */
bool XmlItemParser::checkActionReadTextNode(XmlItemParser::Option option)
{
    bool allowed = false;

    // TODO: add other states?
    if ((m_state == State_ElementStartOfContentRead) ||
        (m_state == State_ElementEndEmptyRead))
    {
        if (option == Option_None)
        {
            allowed = true;
        }
    }

    return allowed;
}

/**
 * Execute state: Reading text node
 *
 * \retval State_ReadingTextNode    Wait for more data
 * \retval State_TextNodeRead       Comment text read
 * \retval State_Error              Error
 *
 * Format:
 * \code{.unparsed}
 * CharData ::= [^<&]* - ([^<&]* ']]>' [^<&]*)
 * \endcode
 *
 * \todo Fully suport the format (references are currently not checked)
 */
XmlItemParser::State XmlItemParser::executeStateReadingTextNode()
{
    State nextState = State_Error;
    bool finishParsing = false;

    do
    {
        // Read more data if needed and check if data is available
        if (readDataIfNeeded() == false)
        {
            // No data available, wait for more data
            nextState = State_ReadingTextNode;
            finishParsing = true;
        }
        else
        {
            // Data available
            const uint32_t unicodeCharacter = m_parsingBuffer.at(m_position);

            // Check if character is valid
            if (unicodeCharacter == (uint32_t)'<')
            {
                m_value = m_parsingBuffer.substr(0U, m_position);
                eraseFromParsingBuffer(m_position);
                m_terminationCharacter = 0U;
                nextState = State_TextNodeRead;
                finishParsing = true;
            }
            else
            {
                m_position++;

                // Check for "]]>" sequence
                if (m_position >= 3U)
                {
                    if ((m_parsingBuffer.at(m_position - 3U) == (uint32_t)']') &&
                        (m_parsingBuffer.at(m_position - 2U) == (uint32_t)']') &&
                        (m_parsingBuffer.at(m_position - 1U) == (uint32_t)'>'))
                    {
                        // Error, invalid sequence
                        m_position = 0U;
                        m_terminationCharacter = unicodeCharacter;
                        finishParsing = true;
                    }
                }
            }
        }
    }
    while (!finishParsing);

    return nextState;
}
