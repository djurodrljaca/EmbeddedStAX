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

#include <EmbeddedStAX/XmlReader/TokenParsers/CommentParser.h>
#include <EmbeddedStAX/XmlValidator/Common.h>

using namespace EmbeddedStAX::XmlReader;

/**
 * Constructor
 *
 * \param parsingBuffer Pointer to a parsing buffer
 */
CommentParser::CommentParser(ParsingBuffer *parsingBuffer)
    : AbstractTokenParser(parsingBuffer, Option_None, ParserType_Comment),
      m_text()
{
}

/**
 * Destructor
 */
CommentParser::~CommentParser()
{
}

/**
 * Check if token parser is valid
 *
 * \retval true     Valid
 * \retval false    Invalid
 */
bool CommentParser::isValid() const
{
    bool valid = AbstractTokenParser::isValid();

    if (valid)
    {
        switch (m_option)
        {
            case Option_None:
            {
                // Valid option
                break;
            }

            default:
            {
                // Invalid option
                valid = false;
                break;
            }
        }
    }

    return valid;
}

/**
 * Parse
 *
 * \retval Result_Success       Success
 * \retval Result_NeedMoreData  More data is needed
 * \retval Result_Error         Error
 *
 * Format:
 * \code{.unparsed}
 * Comment ::= '<!--' ((Char - '-') | ('-' (Char - '-')))* '-->'
 * \endcode
 */
AbstractTokenParser::Result CommentParser::parse()
{
    Result result = Result_Error;

    if (isValid())
    {
        bool finishParsing = false;

        while (!finishParsing)
        {
            finishParsing = true;

            // Check if more data is needed
            if (m_parsingBuffer->isMoreDataNeeded())
            {
                // More data is needed
                result = Result_NeedMoreData;
            }
            else
            {
                // Check for "-->" sequence
                const size_t position = m_parsingBuffer->currentPosition();

                if (position > 1U)
                {
                    const uint32_t minusChar = static_cast<uint32_t>('-');

                    if ((m_parsingBuffer->at(position - 2U) == minusChar) &&
                        (m_parsingBuffer->at(position - 1U) == minusChar))
                    {
                        // Sequence "--" found, now check if '>' char follows it
                        if (m_parsingBuffer->currentChar() == static_cast<uint32_t>('>'))
                        {
                            // End of comment found
                            m_text = m_parsingBuffer->substring(0U, position - 2U);
                            m_parsingBuffer->incrementPosition();
                            result = Result_Success;
                        }
                        else
                        {
                            // Error, invalid character
                        }
                    }
                    else
                    {
                        // Check next character
                        m_parsingBuffer->incrementPosition();
                        finishParsing = false;
                    }
                }
                else
                {
                    // Check next character
                    m_parsingBuffer->incrementPosition();
                    finishParsing = false;
                }
            }
        }
    }

    if ((result == Result_Success) ||
        (result == Result_Error))
    {
        m_parsingBuffer->eraseToCurrentPosition();
    }

    return result;
}

/**
 * Get text string
 *
 * \return Text string
 */
EmbeddedStAX::Common::UnicodeString CommentParser::text() const
{
    return m_text;
}
