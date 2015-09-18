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

#ifndef EMBEDDEDSTAX_XMLREADER_PARSINGBUFFER_H
#define EMBEDDEDSTAX_XMLREADER_PARSINGBUFFER_H

#include <Common/Utf.h>

namespace EmbeddedStAX
{
namespace XmlReader
{
/**
 * Parsing buffer
 *
 * Holds the parsing buffer
 */
class ParsingBuffer
{
public:
    ParsingBuffer();

    size_t size() const;
    void clear();
    void erase(const size_t size);

    uint32_t at(const size_t position) const;
    uint32_t first() const;
    uint32_t current() const;
    bool isMoreDataNeeded() const;

    size_t currentPosition() const;
    bool setCurrentPosition(const size_t position);
    bool incrementPosition();

    Common::UnicodeString substring(const size_t position,
                                    const size_t size = std::string::npos) const;

    size_t writeData(const std::string &data);

private:
    // Private data
    Common::Utf8 m_utf8;
    Common::UnicodeString m_buffer;
    size_t m_position;
};
}
}

#endif // EMBEDDEDSTAX_XMLREADER_PARSINGBUFFER_H
