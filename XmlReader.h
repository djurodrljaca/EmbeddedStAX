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

#ifndef MINISAXCPP_XMLREADER_H
#define MINISAXCPP_XMLREADER_H

#include "Common.h"
#include "XmlItemParser.h"
#include <list>

namespace MiniSaxCpp
{
/**
 * XML Reader class can be used to parse a XML document
 */
class XmlReader
{
public:
    // Public types
    enum ParsingResult
    {
        ParsingResult_Error,
        ParsingResult_NeedMoreData,
        ParsingResult_XmlDeclaration,
        ParsingResult_ProcessingInstruction,
        ParsingResult_DocumentType,
        ParsingResult_Comment,
        ParsingResult_StartOfRootElement,
        ParsingResult_StartOfChildElement,
        ParsingResult_ElementAttribute,
        ParsingResult_EndOfRootElement,
        ParsingResult_EndOfChildElement,
        ParsingResult_TextNode
    };

public:
    XmlReader(const size_t dataBufferSize);

    void clear();
    bool writeData(const char data);

    ParsingResult parse();
    ParsingResult getLastParsingResult();

    std::string getName() const;
    std::string getValue() const;

    bool isXmlDeclarationSet() const;
    bool isXmlDeclarationSupported() const;
    Common::XmlVersion getXmlVersion() const;
    Common::XmlEncoding getXmlEncoding() const;
    Common::XmlStandalone getXmlStandalone() const;

private:
    // Private types
    enum DocumentState
    {
        DocumentState_PrologXmlDeclaration,
        DocumentState_PrologDocumentType,
        DocumentState_PrologOther,
        DocumentState_Document,
        DocumentState_EndOfDocument,
        DocumentState_Error
    };

    enum ParsingState
    {
        ParsingState_Idle,
        ParsingState_ReadingItem,

        ParsingState_ReadingPiTarget,
        ParsingState_ReadingPiValue,
        ParsingState_XmlDeclarationRead,
        ParsingState_ProcessingInstructionRead,

        ParsingState_ReadingDocumentTypeName,
        ParsingState_ReadingDocumentTypeValue,
        ParsingState_DocumentTypeRead,

        ParsingState_ReadingCommentText,
        ParsingState_CommentTextRead,

        ParsingState_ReadingElementName,
        ParsingState_ElementNameRead,

        ParsingState_ReadingElementAttributeName,
        ParsingState_ElementAttributeNameRead,
        ParsingState_ElementStartOfContentRead,
        ParsingState_ElementEndEmptyRead,

        ParsingState_ReadingElementAttributeValue,
        ParsingState_ElementAttributeValueRead,

        ParsingState_ReadingTextNode,
        ParsingState_TextNodeRead,

//        ParsingState_ReadingElementEnd,
//        ParsingState_ElementEndRead,

        ParsingState_Error
    };

private:
    // Private API
    ParsingState executeParsingStateIdle();
    ParsingState executeParsingStateReadingItem();
    ParsingState executeParsingStateReadingPiTarget();
    ParsingState executeParsingStateReadingPiValue();
    ParsingState executeParsingStateReadingDocumentTypeName();
    ParsingState executeParsingStateReadingDocumentTypeValue();
    ParsingState executeParsingStateReadingCommentText();
    ParsingState executeParsingStateReadingElementName();
    ParsingState executeParsingStateReadingElementAttributeName();
    ParsingState executeParsingStateReadingElementAttributeValue();
    ParsingState executeParsingStateReadingTextNode();

    std::string getItemParserValue(bool *success = NULL) const;
    ParsingState evaluateProcessingInstruction();
    bool parseXmlDeclaration();

private:
    // Private data
    DocumentState m_documentState;

    XmlItemParser m_itemParser;
    ParsingState m_parsingState;
    ParsingResult m_lastParsingResult;

    Common::XmlVersion m_xmlVersion;
    Common::XmlEncoding m_xmlEncoding;
    Common::XmlStandalone m_xmlStandalone;

    std::string m_name;
    std::string m_value;

    std::string m_documentTypeName;

    std::list<std::string> m_openedElementNameList;
    std::list<std::string> m_elementAttributeNameList;
};
}

#endif // MINISAXCPP_XMLREADER_H
