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

#ifndef EMBEDDEDSTAX_XMLREADER_TOKENS_ABSTRACTTOKEN_H
#define EMBEDDEDSTAX_XMLREADER_TOKENS_ABSTRACTTOKEN_H

#include <EmbeddedStAX/XmlReader/ParsingBuffer.h>

namespace EmbeddedStAX
{
namespace XmlReader
{
/**
 * Abstract token
 */
class AbstractToken
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

public:
    // Public API
    AbstractToken(ParsingBuffer *parsingBuffer, Option option = Option_None);
    virtual ~AbstractToken() = 0;

    virtual bool isValid() const;
    virtual Result parse() = 0;

protected:
    // Protected data
    ParsingBuffer *m_parsingBuffer;
    Option m_option;
};
}
}

#endif // EMBEDDEDSTAX_XMLREADER_TOKENS_ABSTRACTTOKEN_H
