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

#ifndef EMBEDDEDSTAX_XMLREADER_XMLREADER_H
#define EMBEDDEDSTAX_XMLREADER_XMLREADER_H

#include <EmbeddedStAX/XmlReader/ParsingBuffer.h>
#include <EmbeddedStAX/XmlReader/TokenParsers/AbstractTokenParser.h>
#include <EmbeddedStAX/Common/Attribute.h>
#include <EmbeddedStAX/Common/DocumentType.h>
#include <EmbeddedStAX/Common/ProcessingInstruction.h>
#include <EmbeddedStAX/Common/Utf.h>
#include <EmbeddedStAX/Common/XmlDeclaration.h>
#include <list>

namespace EmbeddedStAX
{
namespace XmlReader
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
        ParsingResult_None,
        ParsingResult_Error,
        ParsingResult_NeedMoreData,
        ParsingResult_XmlDeclaration,
        ParsingResult_ProcessingInstruction,
        ParsingResult_DocumentType,
        ParsingResult_Comment,
        ParsingResult_StartOfElement,
        ParsingResult_EndOfElement,
        ParsingResult_TextNode,
        ParsingResult_CData
    };

public:
    XmlReader();
    ~XmlReader();

    void clear();
    void startNewDocument();
    size_t writeData(const std::string &data);

    ParsingResult parse();
    ParsingResult lastParsingResult();

    Common::XmlDeclaration xmlDeclaration() const;
    Common::ProcessingInstruction processingInstruction() const;
    Common::DocumentType documentType() const;
    Common::UnicodeString text() const;
    Common::UnicodeString name() const;
    std::list<Common::Attribute> attributeList() const;

private:
    // Private types
    enum DocumentState
    {
        DocumentState_PrologWaitForXmlDeclaration,
        DocumentState_PrologWaitForDocumentType,
        DocumentState_PrologWaitForMisc,
        DocumentState_Element,
        DocumentState_EndOfDocument,
        DocumentState_Error
    };

    enum ParsingState
    {
        ParsingState_Idle,
        ParsingState_ReadingTokenType,
        ParsingState_ReadingProcessingInstruction,
        ParsingState_XmlDeclarationRead,
        ParsingState_ProcessingInstructionRead,
        ParsingState_ReadingDocumentType,
        ParsingState_DocumentTypeRead,
        ParsingState_ReadingComment,
        ParsingState_CommentRead,
        ParsingState_ReadingStartOfElement,
        ParsingState_StartOfElementRead,
        ParsingState_EmptyElementRead,

        ParsingState_ReadingEndOfElement,
        ParsingState_EndOfElementRead,

        ParsingState_ReadingTextNode,
        ParsingState_ReadingCData,
        ParsingState_Error
    };

private:
    // Private API
    ParsingState executeParsingStateReadingTokenType();
    ParsingState executeParsingStateReadingProcessingInstruction();
    ParsingState executeParsingStateReadingComment();
    ParsingState executeParsingStateReadingDocumentType();
    ParsingState executeParsingStateReadingStartOfElement();
    // TODO: ParsingState executeParsingStateReadingEndOfElement();
    // TODO: ParsingState executeParsingStateReadingCData();

    bool setTokenParser(AbstractTokenParser *tokenParser);

private:
    // Private data
    DocumentState m_documentState;
    ParsingState m_parsingState;
    ParsingBuffer m_parsingBuffer;
    AbstractTokenParser *m_tokenParser;
    ParsingResult m_lastParsingResult;
    Common::XmlDeclaration m_xmlDeclaration;
    Common::ProcessingInstruction m_processingInstruction;
    Common::DocumentType m_documentType;
    Common::UnicodeString m_text;
    Common::UnicodeString m_name;
    std::list<Common::Attribute> m_attributeList;
    std::list<Common::UnicodeString> m_openElementList;
};
}
}

#endif // EMBEDDEDSTAX_XMLREADER_XMLREADER_H
