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

#ifndef MINISAXCPP_XMLITEMPARSER_H
#define MINISAXCPP_XMLITEMPARSER_H

#include "UnicodeCircularBuffer.h"

namespace MiniSaxCpp
{
/**
 * XML Item Parser class can be used to parse a individual XML items
 *
 * Supported XML items:
 *  - Start of XML item
 *  - End of XML item
 *  - XML item type
 *  - Name
 *  - PI's value
 *  - TODO: add others
 */
class XmlItemParser
{
public:
    // Public types
    enum Result
    {
        Result_NeedMoreData,
        Result_Success,
        Result_Error
    };

    enum Option
    {
        Option_None,
        Option_Synchronization,
        Option_IgnoreLeadingWhitespace
    };

    enum ItemType
    {
        ItemType_None,
        ItemType_Whitespace,
        ItemType_ProcessingInstruction,
        ItemType_DocumentType,
        ItemType_Comment,
        ItemType_Element
    };

    enum Action
    {
        Action_ReadItem,
        Action_ReadName,
        Action_ReadPiValue
    };

public:
    XmlItemParser(const size_t dataBufferSize);

    void clear();
    bool writeData(const char data);

    bool configure(const Action action, const Option option = Option_None);
    Result execute();

    uint32_t getTerminationCharacter();
    ItemType getItemType() const;
    UnicodeString getValue() const;

private:
    // Private types
    enum State
    {
        State_Idle,
        State_WaitForStartOfItem,
        State_WhitespaceFound,
        State_ReadItemType,
        State_ItemTypeFound,

        State_ReadName,
        State_NameFound,

        State_ReadPiValue,
        State_PiValueFound,

        State_Error
    };

private:
    // Private API
    void eraseFromParsingBuffer(const size_t size);
    bool readData();
    bool readDataIfNeeded();

    bool checkActionReadItem(Option option);
    State executeStateWaitForStartOfItem();
    State executeStateReadItemType();

    bool checkActionReadName(Option option);
    State executeStateReadName();

    bool checkActionReadPiValue(Option option);
    State executeStateReadPiValue();

private:
    // Private data
    UnicodeCircularBuffer m_xmlDataBuffer;
    UnicodeString m_parsingBuffer;
    size_t m_position;

    Option m_option;
    State m_state;
    uint32_t m_terminationCharacter;
    ItemType m_itemType;
    UnicodeString m_value;

//    Result parsePiValue();
//    Result parseEndOfPi();
//    Result parseEndOfDocumentType();
//    Result parseComment();

//    bool m_openTagCharacterFound;
};
}

#endif // MINISAXCPP_XMLITEMPARSER_H
