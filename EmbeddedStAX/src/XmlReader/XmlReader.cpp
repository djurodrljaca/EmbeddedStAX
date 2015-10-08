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

#include <EmbeddedStAX/XmlReader/XmlReader.h>

using namespace EmbeddedStAX::XmlReader;

/**
 * Constructor
 */
XmlReader::XmlReader()
    : m_cDataParser(),
      m_commentParser(),
      m_documentTypeParser(),
      m_endOfElementParser(),
      m_processingInstructionParser(),
      m_startOfElementParser(),
      m_textNodeParser(),
      m_tokenTypeParser()
{
    clear();
}

/**
 * Destructor
 */
XmlReader::~XmlReader()
{
    clear();
}

/**
 * Clear internal state
 */
void XmlReader::clear()
{
    m_parsingBuffer.clear();

    startNewDocument();
}

/**
 * Start a new document
 */
void XmlReader::startNewDocument()
{
    m_documentState = DocumentState_PrologWaitForXmlDeclaration;
    m_parsingState = ParsingState_Idle;
    m_lastParsingResult = ParsingResult_None;
    m_parsingBuffer.eraseToCurrentPosition();
    m_xmlDeclaration.clear();
    m_processingInstruction.clear();
    m_documentType.clear();
    m_text.clear();
    m_name.clear();
    m_attributeList.clear();
    m_openElementList.clear();

    m_cDataParser.deinitialize();
    m_commentParser.deinitialize();
    m_documentTypeParser.deinitialize();
    m_endOfElementParser.deinitialize();
    m_processingInstructionParser.deinitialize();
    m_startOfElementParser.deinitialize();
    m_tokenTypeParser.deinitialize();
    m_textNodeParser.deinitialize();
}

/**
 * Write data
 *
 * \param data  Data to write
 *
 * \return Number of character written
 */
size_t XmlReader::writeData(const std::string &data)
{
    return m_parsingBuffer.writeData(data);
}

/**
 * Parse data in the data buffer
 *
 * \return Parsing result
 */
XmlReader::ParsingResult XmlReader::parse()
{
    ParsingResult result = ParsingResult_Error;
    bool finishParsing = false;

    while (!finishParsing)
    {
        finishParsing = true;
        ParsingState nextState = ParsingState_Error;

        switch (m_parsingState)
        {
            case ParsingState_Idle:
            {
                // Start reading a XML document
                if (m_tokenTypeParser.initialize(&m_parsingBuffer))
                {
                    m_documentState = DocumentState_PrologWaitForXmlDeclaration;
                    nextState = ParsingState_ReadingTokenType;
                    finishParsing = false;
                }
                else
                {
                    // Error, failed to initialize parser
                }
                break;
            }

            case ParsingState_ReadingTokenType:
            {
                // Reading token type
                nextState = executeParsingStateReadingTokenType();

                // Check transitions
                switch (nextState)
                {
                    case ParsingState_ReadingTokenType:
                    {
                        // More data is needed
                        result = ParsingResult_NeedMoreData;
                        break;
                    }

                    case ParsingState_ReadingProcessingInstruction:
                    case ParsingState_ReadingDocumentType:
                    case ParsingState_ReadingComment:
                    case ParsingState_ReadingCData:
                    case ParsingState_ReadingStartOfElement:
                    case ParsingState_ReadingEndOfElement:
                    {
                        // Execute another cycle
                        finishParsing = false;
                        break;
                    }

                    default:
                    {
                        // Error
                        nextState = ParsingState_Error;
                        break;
                    }
                }
                break;
            }

            case ParsingState_ReadingProcessingInstruction:
            {
                // Reading processing instruction
                nextState = executeParsingStateReadingProcessingInstruction();

                // Check transitions
                switch (nextState)
                {
                    case ParsingState_ReadingProcessingInstruction:
                    {
                        // More data is needed
                        result = ParsingResult_NeedMoreData;
                        break;
                    }

                    case ParsingState_XmlDeclarationRead:
                    {
                        result = ParsingResult_XmlDeclaration;
                        break;
                    }

                    case ParsingState_ProcessingInstructionRead:
                    {
                        result = ParsingResult_ProcessingInstruction;
                        break;
                    }

                    default:
                    {
                        // Error
                        nextState = ParsingState_Error;
                        break;
                    }
                }
                break;
            }

            case ParsingState_ReadingComment:
            {
                // Reading comment
                nextState = executeParsingStateReadingComment();

                // Check transitions
                switch (nextState)
                {
                    case ParsingState_ReadingComment:
                    {
                        // More data is needed
                        result = ParsingResult_NeedMoreData;
                        break;
                    }

                    case ParsingState_CommentRead:
                    {
                        result = ParsingResult_Comment;
                        break;
                    }

                    default:
                    {
                        // Error
                        nextState = ParsingState_Error;
                        break;
                    }
                }
                break;
            }

            case ParsingState_ReadingDocumentType:
            {
                // Reading document type
                nextState = executeParsingStateReadingDocumentType();

                // Check transitions
                switch (nextState)
                {
                    case ParsingState_ReadingDocumentType:
                    {
                        // More data is needed
                        result = ParsingResult_NeedMoreData;
                        break;
                    }

                    case ParsingState_DocumentTypeRead:
                    {
                        result = ParsingResult_DocumentType;
                        break;
                    }

                    default:
                    {
                        // Error
                        nextState = ParsingState_Error;
                        break;
                    }
                }
                break;
            }

            case ParsingState_ReadingStartOfElement:
            {
                // Reading element type
                nextState = executeParsingStateReadingStartOfElement();

                // Check transitions
                switch (nextState)
                {
                    case ParsingState_ReadingStartOfElement:
                    {
                        // More data is needed
                        result = ParsingResult_NeedMoreData;
                        break;
                    }

                    case ParsingState_StartOfElementRead:
                    {
                        if (m_openElementList.size() >= 1U)
                        {
                            result = ParsingResult_StartOfElement;
                        }
                        else
                        {
                            // Error, there should be at least 1 open element
                            nextState = ParsingState_Error;
                        }
                        break;
                    }

                    case ParsingState_EmptyElementRead:
                    {
                        result = ParsingResult_StartOfElement;
                        break;
                    }

                    default:
                    {
                        // Error
                        nextState = ParsingState_Error;
                        break;
                    }
                }
                break;
            }

            case ParsingState_ReadingTextNode:
            {
                // Reading text node
                nextState = executeParsingStateReadingTextNode();

                // Check transitions
                switch (nextState)
                {
                    case ParsingState_ReadingTextNode:
                    {
                        // More data is needed
                        result = ParsingResult_NeedMoreData;
                        break;
                    }

                    case ParsingState_TextNodeRead:
                    {
                        // Check if any text was read
                        if (m_text.empty())
                        {
                            // No text was read, continue parsing
                            finishParsing = false;
                        }
                        else
                        {
                            // Text was read
                            result = ParsingResult_TextNode;
                        }
                        break;
                    }

                    default:
                    {
                        // Error
                        nextState = ParsingState_Error;
                        break;
                    }
                }
                break;
            }

            case ParsingState_ReadingCData:
            {
                // Reading CDATA
                nextState = executeParsingStateReadingCData();

                // Check transitions
                switch (nextState)
                {
                    case ParsingState_ReadingCData:
                    {
                        // More data is needed
                        result = ParsingResult_NeedMoreData;
                        break;
                    }

                    case ParsingState_CDataRead:
                    {
                        // CDATA was read
                        result = ParsingResult_CData;
                        break;
                    }

                    default:
                    {
                        // Error
                        nextState = ParsingState_Error;
                        break;
                    }
                }
                break;
            }

            case ParsingState_ReadingEndOfElement:
            {
                // Reading end of element
                nextState = executeParsingStateReadingEndOfElement();

                // Check transitions
                switch (nextState)
                {
                    case ParsingState_ReadingEndOfElement:
                    {
                        // More data is needed
                        result = ParsingResult_NeedMoreData;
                        break;
                    }

                    case ParsingState_EndOfElementRead:
                    {
                        // Check for end of root element
                        if (m_openElementList.empty())
                        {
                            // End of root element, document is finished
                            m_documentState = DocumentState_EndOfDocument;
                        }

                        // End of element was read
                        nextState = ParsingState_EndOfElementRead;
                        result = ParsingResult_EndOfElement;
                        break;
                    }

                    default:
                    {
                        // Error
                        nextState = ParsingState_Error;
                        break;
                    }
                }
                break;
            }

            case ParsingState_EmptyElementRead:
            {
                // Check for end of root element
                if (m_openElementList.empty())
                {
                    // End of root element, document is finished
                    m_documentState = DocumentState_EndOfDocument;
                }

                // Name must not be cleared (it holds the name of the "closed" empty element), but
                // the attributes can be cleared
                m_attributeList.clear();

                // For an empty element, just return the "end of element" result
                nextState = ParsingState_EndOfElementRead;
                result = ParsingResult_EndOfElement;
                break;
            }

            case ParsingState_CDataRead:
            {
                m_text.clear();

                // Start reading next token
                if (m_textNodeParser.initialize(&m_parsingBuffer))
                {
                    // Read token type
                    nextState = ParsingState_ReadingTextNode;
                    finishParsing = false;
                }
                else
                {
                    // Error, failed to initialize parser
                }
                break;
            }

            case ParsingState_StartOfElementRead:
            {
                m_name.clear();
                m_attributeList.clear();

                // Start reading next token
                if (m_textNodeParser.initialize(&m_parsingBuffer))
                {
                    // Read token type
                    nextState = ParsingState_ReadingTextNode;
                    finishParsing = false;
                }
                else
                {
                    // Error, failed to initialize parser
                }
                break;
            }

            case ParsingState_TextNodeRead:
            {
                m_text.clear();

                // Start reading next token
                if (m_tokenTypeParser.initialize(&m_parsingBuffer,
                                                 TokenTypeParser::Option_IgnoreLeadingWhitespace))
                {
                    // Read token type
                    nextState = ParsingState_ReadingTokenType;
                    finishParsing = false;
                }
                else
                {
                    // Error, failed to initialize parser
                }
                break;
            }

            case ParsingState_DocumentTypeRead:
            case ParsingState_XmlDeclarationRead:
            {
                // Start reading next token
                if (m_tokenTypeParser.initialize(&m_parsingBuffer,
                                                 TokenTypeParser::Option_IgnoreLeadingWhitespace))
                {
                    // Read token type
                    nextState = ParsingState_ReadingTokenType;
                    finishParsing = false;
                }
                else
                {
                    // Error, failed to initialize parser
                }
                break;
            }

            case ParsingState_CommentRead:
            {
                m_text.clear();

                if (m_documentState == DocumentState_Element)
                {
                    // Start reading next token
                    if (m_textNodeParser.initialize(&m_parsingBuffer))
                    {
                        // Read token type
                        nextState = ParsingState_ReadingTextNode;
                        finishParsing = false;
                    }
                    else
                    {
                        // Error, failed to initialize parser
                    }
                }
                else
                {
                    // Start reading next token
                    const TokenTypeParser::Option option =
                            TokenTypeParser::Option_IgnoreLeadingWhitespace;

                    if (m_tokenTypeParser.initialize(&m_parsingBuffer, option))
                    {
                        // Read token type
                        nextState = ParsingState_ReadingTokenType;
                        finishParsing = false;
                    }
                    else
                    {
                        // Error, failed to initialize parser
                    }
                }
                break;
            }

            case ParsingState_EndOfElementRead:
            {
                m_name.clear();

                if (m_documentState == DocumentState_Element)
                {
                    // Start reading next token
                    if (m_textNodeParser.initialize(&m_parsingBuffer))
                    {
                        // Read token type
                        nextState = ParsingState_ReadingTextNode;
                        finishParsing = false;
                    }
                    else
                    {
                        // Error, failed to initialize parser
                    }
                }
                else
                {
                    // Start reading next token
                    const TokenTypeParser::Option option =
                            TokenTypeParser::Option_IgnoreLeadingWhitespace;

                    if (m_tokenTypeParser.initialize(&m_parsingBuffer, option))
                    {
                        // Read token type
                        nextState = ParsingState_ReadingTokenType;
                        finishParsing = false;
                    }
                    else
                    {
                        // Error, failed to initialize parser
                    }
                }
                break;
            }

            case ParsingState_ProcessingInstructionRead:
            {
                m_processingInstruction.clear();

                if (m_documentState == DocumentState_Element)
                {
                    // Start reading next token
                    if (m_textNodeParser.initialize(&m_parsingBuffer))
                    {
                        // Read token type
                        nextState = ParsingState_ReadingTextNode;
                        finishParsing = false;
                    }
                    else
                    {
                        // Error, failed to initialize parser
                    }
                }
                else
                {
                    // Start reading next token
                    const TokenTypeParser::Option option =
                            TokenTypeParser::Option_IgnoreLeadingWhitespace;

                    if (m_tokenTypeParser.initialize(&m_parsingBuffer, option))
                    {
                        // Read token type
                        nextState = ParsingState_ReadingTokenType;
                        finishParsing = false;
                    }
                    else
                    {
                        // Error, failed to initialize parser
                    }
                }
                break;
            }

            default:
            {
                // Error
                nextState = ParsingState_Error;
                break;
            }
        }

        // Update parsing state
        m_parsingState = nextState;

        if (m_parsingState == ParsingState_Error)
        {
            m_documentState == DocumentState_Error;
        }
    }

    // Save last parsing result
    m_lastParsingResult = result;
    return result;
}

/**
 * Get last parsing result
 *
 * \return Last parsing result
 */
XmlReader::ParsingResult XmlReader::lastParsingResult()
{
    return m_lastParsingResult;
}

/**
 * Get XML declaration
 *
 * \return XML declaration
 */
EmbeddedStAX::Common::XmlDeclaration XmlReader::xmlDeclaration() const
{
    return m_xmlDeclaration;
}

/**
 * Get processing instruction
 *
 * \return Processing instruction
 */
EmbeddedStAX::Common::ProcessingInstruction XmlReader::processingInstruction() const
{
    return m_processingInstruction;
}

/**
 * Get document type
 *
 * \return Document type
 */
EmbeddedStAX::Common::DocumentType XmlReader::documentType() const
{
    return m_documentType;
}

/**
 * Get text. Value depends on the parsing result. It can contain:
 * - Comment Text
 * - Text Node
 * - CDATA
 *
 * \return Text
 */
EmbeddedStAX::Common::UnicodeString XmlReader::text() const
{
    return m_text;
}

/**
 * Get element name
 *
 * \return Element name
 */
EmbeddedStAX::Common::UnicodeString XmlReader::name() const
{
    return m_name;
}

/**
 * Get attribute list
 *
 * \return Attribute list
 */
EmbeddedStAX::Common::AttributeList XmlReader::attributeList() const
{
    return m_attributeList;
}

/**
 * Execute parsing state: Reading token type
 *
 * \retval ParsingState_ReadingTokenType                Wait for more data
 * \retval ParsingState_ReadingProcessingInstruction    Processing instruction token found
 * \retval ParsingState_ReadingDocumentType             Document type token found
 * \retval ParsingState_ReadingComment                  Comment token found
 * \retval ParsingState_ReadingCData                    CDATA token found
 * \retval ParsingState_ReadingStartOfElement           Start of element token found
 * \retval ParsingState_ReadingEndOfElement             End of element token found
 * \retval ParsingState_Error                           Error
 */
XmlReader::ParsingState XmlReader::executeParsingStateReadingTokenType()
{
    ParsingState nextState = ParsingState_Error;
    bool finishParsing = false;

    while (!finishParsing)
    {
        finishParsing = true;

        // Parse
        const TokenTypeParser::Result result = m_tokenTypeParser.parse();

        switch (result)
        {
            case TokenTypeParser::Result_NeedMoreData:
            {
                // More data is needed
                nextState = ParsingState_ReadingTokenType;
                break;
            }

            case TokenTypeParser::Result_Success:
            {
                // Check token type
                const TokenTypeParser::TokenType tokenType = m_tokenTypeParser.tokenType();

                switch (tokenType)
                {
                    case TokenTypeParser::TokenType_Whitespace:
                    {
                        // Check document state
                        if (m_documentState == DocumentState_PrologWaitForXmlDeclaration)
                        {
                            // The first character does not start a XML declaration, so we should no
                            // longer wait for one. Start waiting for document type instead.
                            m_documentState = DocumentState_PrologWaitForDocumentType;
                        }

                        // Reconfigure parset to ignore leading whitespaces
                        const TokenTypeParser::Option option =
                                TokenTypeParser::Option_IgnoreLeadingWhitespace;

                        if (m_tokenTypeParser.initialize(&m_parsingBuffer, option))
                        {
                            // Execute another cycle
                            finishParsing = false;
                        }
                        else
                        {
                            // Error, failed to initialize parser
                        }
                        break;
                    }

                    case TokenTypeParser::TokenType_ProcessingInstruction:
                    {
                        // Set procesing instruction parser
                        if (m_processingInstructionParser.initialize(&m_parsingBuffer))
                        {
                            // Processing instruction token found
                            nextState = ParsingState_ReadingProcessingInstruction;
                        }
                        else
                        {
                            // Error, failed to initialize parser
                        }
                        break;
                    }

                    case TokenTypeParser::TokenType_DocumentType:
                    {
                        // Set document type parser
                        if (m_documentTypeParser.initialize(&m_parsingBuffer))
                        {
                            // Check document state
                            if (m_documentState == DocumentState_PrologWaitForXmlDeclaration)
                            {
                                // The first characters do not start a XML declaration, so we should
                                // no longer wait for one. Start waiting for document type instead.
                                m_documentState = DocumentState_PrologWaitForDocumentType;

                                // Document type token found
                                nextState = ParsingState_ReadingDocumentType;
                            }
                            else if (m_documentState == DocumentState_PrologWaitForDocumentType)
                            {
                                // Document type token found
                                nextState = ParsingState_ReadingDocumentType;
                            }
                            else
                            {
                                // Error, document type is not allowed in the current document state
                            }
                        }
                        else
                        {
                            // Error, failed to initialize parser
                        }
                        break;
                    }

                    case TokenTypeParser::TokenType_Comment:
                    {
                        // Set comment parser
                        if (m_commentParser.initialize(&m_parsingBuffer))
                        {
                            // Check document state
                            if (m_documentState == DocumentState_PrologWaitForXmlDeclaration)
                            {
                                // The first characters do not start a XML declaration, so we should
                                // no longer wait for one. Start waiting for document type instead.
                                m_documentState = DocumentState_PrologWaitForDocumentType;
                            }

                            // Comment token found
                            nextState = ParsingState_ReadingComment;
                        }
                        else
                        {
                            // Error, failed to initialize parser
                        }
                        break;
                    }

                    case TokenTypeParser::TokenType_CData:
                    {
                        // Set comment parser
                        if (m_cDataParser.initialize(&m_parsingBuffer))
                        {
                            // Check document state
                            if (m_documentState == DocumentState_Element)
                            {
                                // CDATA token found
                                nextState = ParsingState_ReadingCData;
                            }
                            else
                            {
                                // Error, CDATA is only allowed inside an element
                            }
                        }
                        else
                        {
                            // Error, failed to initialize parser
                        }
                        break;
                    }

                    case TokenTypeParser::TokenType_StartOfElement:
                    {
                        // Start of element token found
                        if (m_startOfElementParser.initialize(&m_parsingBuffer))
                        {
                            m_name.clear();
                            m_attributeList.clear();

                            // Check document state
                            if ((m_documentState == DocumentState_PrologWaitForXmlDeclaration) ||
                                (m_documentState == DocumentState_PrologWaitForDocumentType) ||
                                (m_documentState == DocumentState_PrologWaitForMisc) ||
                                (m_documentState == DocumentState_Element))
                            {
                                // End of element token found
                                m_documentState = DocumentState_Element;
                                nextState = ParsingState_ReadingStartOfElement;
                            }
                            else
                            {
                                // Error, end of element is only allowed at the end of an open
                                // element
                            }
                        }
                        else
                        {
                            // Error, failed to initialize parser
                        }
                        break;
                    }

                    case TokenTypeParser::TokenType_EndOfElement:
                    {
                        if (m_endOfElementParser.initialize(&m_parsingBuffer))
                        {
                            m_name.clear();

                            // Check document state
                            if (m_documentState == DocumentState_Element)
                            {
                                // End of element token found
                                nextState = ParsingState_ReadingEndOfElement;
                            }
                            else
                            {
                                // Error, end of element is only allowed at the end of an open
                                // element
                            }
                        }
                        else
                        {
                            // Error, failed to initialize parser
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

            default:
            {
                // Error
                break;
            }
        }
    }

    return nextState;
}

/**
 * Execute parsing state: Reading processing instruction
 *
 * \retval ParsingState_ReadingProcessingInstruction    Wait for more data
 * \retval ParsingState_ProcessingInstructionRead       Processing instruction was read
 * \retval ParsingState_XmlDeclarationRead              XML declaration was read
 * \retval ParsingState_Error                           Error
 */
XmlReader::ParsingState XmlReader::executeParsingStateReadingProcessingInstruction()
{
    ParsingState nextState = ParsingState_Error;

    // Parse
    const ProcessingInstructionParser::Result result = m_processingInstructionParser.parse();

    switch (result)
    {
        case ProcessingInstructionParser::Result_NeedMoreData:
        {
            // More data is needed
            nextState = ParsingState_ReadingProcessingInstruction;
            break;
        }

        case ProcessingInstructionParser::Result_Success:
        {
            // Check token type
            const ProcessingInstructionParser::TokenType tokenType =
                    m_processingInstructionParser.tokenType();

            switch (tokenType)
            {
                case ProcessingInstructionParser::TokenType_ProcessingInstruction:
                {
                    // Processing instruction read
                    m_processingInstruction = m_processingInstructionParser.processingInstruction();

                    // Check document state
                    if (m_documentState == DocumentState_PrologWaitForXmlDeclaration)
                    {
                        // A processing instruction was fround instead of a XML
                        // declaration at the start of the document. Now start waiting
                        // for document type.
                        m_documentState = DocumentState_PrologWaitForDocumentType;
                    }

                    nextState = ParsingState_ProcessingInstructionRead;
                    break;
                }

                case ProcessingInstructionParser::TokenType_XmlDeclaration:
                {
                    // Check document state
                    if (m_documentState == DocumentState_PrologWaitForXmlDeclaration)
                    {
                        // XML declaration read
                        m_xmlDeclaration = m_processingInstructionParser.xmlDeclaration();

                        // A XML declaration is at the start of the document. Now start
                        // waiting for document typel.
                        m_documentState = DocumentState_PrologWaitForDocumentType;
                        nextState = ParsingState_XmlDeclarationRead;
                    }
                    else
                    {
                        // Error, XML declaration was read at the unexpected time
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

        default:
        {
            // Error
            break;
        }
    }

    return nextState;
}

/**
 * Execute parsing state: Reading comment
 *
 * \retval ParsingState_ReadingComment  Wait for more data
 * \retval ParsingState_CommentRead     Processing instruction was read
 * \retval ParsingState_Error           Error
 */
XmlReader::ParsingState XmlReader::executeParsingStateReadingComment()
{
    ParsingState nextState = ParsingState_Error;

    // Parse
    const CommentParser::Result result = m_commentParser.parse();

    switch (result)
    {
        case CommentParser::Result_NeedMoreData:
        {
            // More data is needed
            nextState = ParsingState_ReadingComment;
            break;
        }

        case CommentParser::Result_Success:
        {
            // Save comment text
            m_text = m_commentParser.text();

            // Check document state
            if (m_documentState == DocumentState_PrologWaitForXmlDeclaration)
            {
                // A comment was fround instead of a XML declaration at the start of the
                // document. Now start waiting for document type.
                m_documentState = DocumentState_PrologWaitForDocumentType;
            }

            nextState = ParsingState_CommentRead;
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
 * Execute parsing state: Reading document type
 *
 * \retval ParsingState_ReadingDocumentType Wait for more data
 * \retval ParsingState_DocumentTypeRead    Document type was read
 * \retval ParsingState_Error               Error
 */
XmlReader::ParsingState XmlReader::executeParsingStateReadingDocumentType()
{
    ParsingState nextState = ParsingState_Error;

    // Parse
    const DocumentTypeParser::Result result = m_documentTypeParser.parse();

    switch (result)
    {
        case DocumentTypeParser::Result_NeedMoreData:
        {
            // More data is needed
            nextState = ParsingState_ReadingDocumentType;
            break;
        }

        case DocumentTypeParser::Result_Success:
        {
            // Document type read
            m_documentType = m_documentTypeParser.documentType();

            // Check document state
            if (m_documentState == DocumentState_PrologWaitForDocumentType)
            {
                // A document type was fround. Now start waiting for Misc
                m_documentState = DocumentState_PrologWaitForMisc;
                nextState = ParsingState_DocumentTypeRead;
            }
            else
            {
                // Error, invalid document state
            }
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
 * Execute parsing state: Reading start of element
 *
 * \retval ParsingState_ReadingStartOfElement   Wait for more data
 * \retval ParsingState_StartOfElementRead      Start of element was read
 * \retval ParsingState_EmptyElementRead        Empty element was read
 * \retval ParsingState_Error                   Error
 */
XmlReader::ParsingState XmlReader::executeParsingStateReadingStartOfElement()
{
    ParsingState nextState = ParsingState_Error;

    // Parse
    const StartOfElementParser::Result result = m_startOfElementParser.parse();

    switch (result)
    {
        case StartOfElementParser::Result_NeedMoreData:
        {
            // More data is needed
            nextState = ParsingState_ReadingStartOfElement;
            break;
        }

        case StartOfElementParser::Result_Success:
        {
            // Check document state
            switch (m_documentState)
            {
                case DocumentState_PrologWaitForXmlDeclaration:
                case DocumentState_PrologWaitForDocumentType:
                case DocumentState_PrologWaitForMisc:
                case DocumentState_Element:
                {
                    // Start of element read
                    m_name = m_startOfElementParser.name();
                    m_attributeList = m_startOfElementParser.attributeList();

                    if (m_documentState != DocumentState_Element)
                    {
                        m_documentState = DocumentState_Element;
                    }

                    const StartOfElementParser::TokenType tokenType =
                            m_startOfElementParser.tokenType();

                    switch (tokenType)
                    {
                        case StartOfElementParser::TokenType_StartOfElement:
                        {
                            // Check for start of root element
                            bool success = true;

                            if (m_openElementList.empty())
                            {
                                const Common::UnicodeString rootName = m_documentType.name();

                                if (!rootName.empty())
                                {
                                    // Root element name was set in the document type, make sure
                                    // that the root element name matches
                                    if (m_name != rootName)
                                    {
                                        // Error, root element name does not match the root
                                        // element name from the document type
                                        success = false;
                                    }
                                }
                            }

                            m_openElementList.push_back(m_name);
                            nextState = ParsingState_StartOfElementRead;
                            break;
                        }

                        case StartOfElementParser::TokenType_EmptyElement:
                        {
                            nextState = ParsingState_EmptyElementRead;
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

                default:
                {
                    // Error, invalid state
                    break;
                }
            }
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
 * Execute parsing state: Reading text node
 *
 * \retval ParsingState_ReadingTextNode Wait for more data
 * \retval ParsingState_TextNodeRead    Text node was read
 * \retval ParsingState_Error           Error
 */
XmlReader::ParsingState XmlReader::executeParsingStateReadingTextNode()
{
    ParsingState nextState = ParsingState_Error;

    // Parse
    const TextNodeParser::Result result = m_textNodeParser.parse();

    switch (result)
    {
        case TextNodeParser::Result_NeedMoreData:
        {
            // More data is needed
            nextState = ParsingState_ReadingTextNode;
            break;
        }

        case TextNodeParser::Result_Success:
        {
            // Save text node
            m_text = m_textNodeParser.text();
            nextState = ParsingState_TextNodeRead;
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
 * Execute parsing state: Reading CDATA
 *
 * \retval ParsingState_ReadingCData    Wait for more data
 * \retval ParsingState_CDataRead       CDATA was read
 * \retval ParsingState_Error           Error
 */
XmlReader::ParsingState XmlReader::executeParsingStateReadingCData()
{
    ParsingState nextState = ParsingState_Error;

    // Parse
    const CDataParser::Result result = m_cDataParser.parse();

    switch (result)
    {
        case CDataParser::Result_NeedMoreData:
        {
            // More data is needed
            nextState = ParsingState_ReadingCData;
            break;
        }

        case CDataParser::Result_Success:
        {
            // Save CDATA text
            m_text = m_cDataParser.text();
            m_cDataParser.deinitialize();
            nextState = ParsingState_CDataRead;
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
 * Execute parsing state: Reading end of element
 *
 * \retval ParsingState_ReadingEndOfElement Wait for more data
 * \retval ParsingState_EndOfElementRead    End of element was read
 * \retval ParsingState_Error               Error
 */
XmlReader::ParsingState XmlReader::executeParsingStateReadingEndOfElement()
{
    ParsingState nextState = ParsingState_Error;

    // Parse
    const EndOfElementParser::Result result = m_endOfElementParser.parse();

    switch (result)
    {
        case EndOfElementParser::Result_NeedMoreData:
        {
            // More data is needed
            nextState = ParsingState_ReadingEndOfElement;
            break;
        }

        case EndOfElementParser::Result_Success:
        {
            // End of element read
            m_name = m_endOfElementParser.name();

            // Check if end of element matches currently open element
            if (m_name == m_openElementList.back())
            {
                // Element name matches
                m_openElementList.pop_back();
                nextState = ParsingState_EndOfElementRead;
            }
            else
            {
                // Error
            }
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
