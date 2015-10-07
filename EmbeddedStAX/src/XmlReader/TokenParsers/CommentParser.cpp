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
 */
CommentParser::CommentParser()
    : AbstractTokenParser(ParserType_Comment),
      m_state(State_ReadingComment),
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
 * Get text string
 *
 * \return Text string
 */
EmbeddedStAX::Common::UnicodeString CommentParser::text() const
{
    return m_text;
}

/**
 * Parse
 *
 * \retval Result_Success       Success
 * \retval Result_NeedMoreData  More data is needed
 * \retval Result_Error         Error
 */
AbstractTokenParser::Result CommentParser::parse()
{
    Result result = Result_Error;

    if (isInitialized())
    {
        bool finishParsing = false;

        while (!finishParsing)
        {
            finishParsing = true;
            State nextState = State_Error;

            switch (m_state)
            {
                case State_ReadingComment:
                {
                    // Reading Comment
                    nextState = executeStateReadingComment();

                    // Check transitions
                    switch (nextState)
                    {
                        case State_ReadingComment:
                        {
                            result = Result_NeedMoreData;
                            break;
                        }

                        case State_Finished:
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

                case State_Finished:
                {
                    result = Result_Success;
                    break;
                }

                default:
                {
                    // Error, invalid state
                    nextState = State_Error;
                    break;
                }
            }

            // Update state
            m_state = nextState;
        }
    }

    if ((result == Result_Success) ||
        (result == Result_Error))
    {
        parsingBuffer()->eraseToCurrentPosition();
    }

    return result;
}

/**
 * Initialize parser's additional data
 *
 * \retval true     Success
 * \retval false    Error
 */
bool CommentParser::initializeAdditionalData()
{
    m_state = State_ReadingComment;
    m_text.clear();
    parsingBuffer()->eraseToCurrentPosition();
    return true;
}

/**
 * Deinitialize parser's additional data
 */
void CommentParser::deinitializeAdditionalData()
{
    m_state = State_ReadingComment;
    m_text.clear();
}

/**
 * Execute state: Reading comment
 *
 * \retval State_ReadingComment Wait for more data
 * \retval State_Finished       Comment found
 * \retval State_Error          Error, unexpected character
 *
 * Format:
 * \code{.unparsed}
 * Comment ::= '<!--' ((Char - '-') | ('-' (Char - '-')))* '-->'
 * \endcode
 */
CommentParser::State CommentParser::executeStateReadingComment()
{
    State nextState = State_Error;
    bool finishParsing = false;

    while (!finishParsing)
    {
        finishParsing = true;

        // Check if more data is needed
        if (parsingBuffer()->isMoreDataNeeded())
        {
            // More data is needed
            nextState = State_ReadingComment;
        }
        else
        {
            // Check for "-->" sequence
            const size_t position = parsingBuffer()->currentPosition();

            if (position > 1U)
            {
                const uint32_t minusChar = static_cast<uint32_t>('-');

                if ((parsingBuffer()->at(position - 2U) == minusChar) &&
                    (parsingBuffer()->at(position - 1U) == minusChar))
                {
                    // Sequence "--" found, now check if '>' char follows it
                    if (parsingBuffer()->currentChar() == static_cast<uint32_t>('>'))
                    {
                        // End of comment found
                        m_text = parsingBuffer()->substring(0U, position - 2U);
                        parsingBuffer()->incrementPosition();
                        nextState = State_Finished;
                    }
                    else
                    {
                        // Error, invalid character
                    }
                }
                else
                {
                    // Check next character
                    parsingBuffer()->incrementPosition();
                    finishParsing = false;
                }
            }
            else
            {
                // Check next character
                parsingBuffer()->incrementPosition();
                finishParsing = false;
            }
        }
    }

    return nextState;
}
