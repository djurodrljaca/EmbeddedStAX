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
#include <EmbeddedStAX/XmlReader/TokenParsers/TokenTypeParser.h>

using namespace EmbeddedStAX::XmlReader;

/**
 * Constructor
 */
XmlReader::XmlReader()
    : m_tokenParser(NULL)
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

    if (m_tokenParser != NULL)
    {
        delete m_tokenParser;
        m_tokenParser = NULL;
    }
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
                // Idle
                nextState = executeParsingStateIdle();

                // Check transitions
                switch (nextState)
                {
                    case ParsingState_ReadingTokenType:
                    {
                        // Execute another cycle
                        finishParsing = false;
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
                        break;
                    }
                }
                break;
            }

                // TODO: ParsingState_ReadingProcessingInstruction
                // TODO: ParsingState_ReadingDocumentType
                // TODO: ParsingState_ReadingComment
                // TODO: ParsingState_ReadingCData
                // TODO: ParsingState_ReadingStartOfElement
                // TODO: ParsingState_ReadingEndOfElement

            default:
            {
                break;
            }
        }

        // Update parsing state
        m_parsingState = nextState;
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
 * Execute parsing state: Idle
 *
 * \retval ParsingState_ReadingTokenType    Token parser for reading token type created
 * \retval ParsingState_Error               Error
 */
XmlReader::ParsingState XmlReader::executeParsingStateIdle()
{
    ParsingState nextState = ParsingState_Error;

    // Create token parser for reading token type
    bool success = setTokenParser(new TokenTypeParser(&m_parsingBuffer));

    if (success)
    {
        // Start reading a XML document
        m_documentState = DocumentState_PrologWaitForXmlDeclaration;
        nextState = ParsingState_ReadingTokenType;
    }

    return nextState;
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
        const AbstractTokenParser::Result result = m_tokenParser->parse();

        switch (result)
        {
            case AbstractTokenParser::Result_NeedMoreData:
            {
                // More data is needed
                nextState = ParsingState_ReadingTokenType;
                break;
            }

            case AbstractTokenParser::Result_Success:
            {
                // Check token type
                const AbstractTokenParser::TokenType tokenType = m_tokenParser->tokenFound();

                switch (tokenType)
                {
                    case AbstractTokenParser::TokenType_Whitespace:
                    {
                        // Check document state
                        if (m_documentState == DocumentState_PrologWaitForXmlDeclaration)
                        {
                            // The first character does not start a XML declaration, so we should no
                            // longer wait for one. Start waiting for document type instead.
                            m_documentState = DocumentState_PrologWaitForDocumentType;
                        }

                        // Reconfigure parset to ignore leading whitespaces
                        if (m_tokenParser->setOption(
                                AbstractTokenParser::Option_IgnoreLeadingWhitespace))
                        {
                            // Execute another cycle
                            finishParsing = false;
                        }
                        break;
                    }

                    case AbstractTokenParser::TokenType_ProcessingInstruction:
                    {
                        // Processing instruction token found
                        nextState = ParsingState_ReadingProcessingInstruction;
                        break;
                    }

                    case AbstractTokenParser::TokenType_DocumentType:
                    {
                        // Check document state
                        if (m_documentState == DocumentState_PrologWaitForXmlDeclaration)
                        {
                            // The first characters do not start a XML declaration, so we should no
                            // longer wait for one. Start waiting for document type instead.
                            m_documentState = DocumentState_PrologWaitForDocumentType;
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
                        break;
                    }

                    case AbstractTokenParser::TokenType_Comment:
                    {
                        // Check document state
                        if (m_documentState == DocumentState_PrologWaitForXmlDeclaration)
                        {
                            // The first characters do not start a XML declaration, so we should no
                            // longer wait for one. Start waiting for document type instead.
                            m_documentState = DocumentState_PrologWaitForDocumentType;
                        }

                        // Comment token found
                        nextState = ParsingState_ReadingComment;
                        break;
                    }

                    case AbstractTokenParser::TokenType_CData:
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
                        break;
                    }

                    case AbstractTokenParser::TokenType_StartOfElement:
                    {
                        // Start of element token found
                        nextState = ParsingState_ReadingStartOfElement;
                        break;
                    }

                    case AbstractTokenParser::TokenType_EndOfElement:
                    {
                        // Check document state
                        if (m_documentState == DocumentState_Element)
                        {
                            // End of element token found
                            nextState = ParsingState_ReadingEndOfElement;
                        }
                        else
                        {
                            // Error, end of element is only allowed at the end of an open element
                        }
                        break;
                    }

                    default:
                    {
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
 * Set token parser
 *
 * \param tokenParser   New token parser
 *
 * \retval true     Success
 * \retval false    Error
 */
bool XmlReader::setTokenParser(AbstractTokenParser *tokenParser)
{
    bool success = false;

    if (tokenParser != NULL)
    {
        if (tokenParser->isValid())
        {
            if (m_tokenParser != NULL)
            {
                delete m_tokenParser;
            }

            m_tokenParser = tokenParser;
            success = true;
        }
    }

    return success;
}

















///**
// * Execute parsing state machine state: Waiting for start of item
// *
// * \retval ParsingState_WaitingForStartOfItem   Waiting for more data
// * \retval ParsingState_ReadingItemType         Start of item found
// */
//XmlReader::ParsingState XmlReader::executeParsingStateIdle()
//{
//    ParsingState nextState = ParsingState_Error;

//    // Prepare parsing option
//    XmlItemParser::Option option = XmlItemParser::Option_None;

//    if (m_documentState == DocumentState_PrologDocumentType)
//    {
//        option = XmlItemParser::Option_IgnoreLeadingWhitespace;
//    }

//    // Initiate reading of an item
//    if (m_itemParser.configure(XmlItemParser::Action_ReadItem, option))
//    {
//        nextState = ParsingState_ReadingItem;
//    }

//    return nextState;
//}
