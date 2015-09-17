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

#ifndef MINISAXCPP_UNICODECIRCULARBUFFER_H
#define MINISAXCPP_UNICODECIRCULARBUFFER_H

#include "Utf.h"
#include <string>
#include <stdint.h>

namespace EmbeddedStAX
{
/**
 * A circular buffer of unicode characters
 *
 * \note This class only accepts ordinary character data and then it internally converts them to
 *       Unicode characters.
 */
class UnicodeCircularBuffer
{
public:
    // Public API
    UnicodeCircularBuffer(const size_t size);
    ~UnicodeCircularBuffer();

    void clear();
    bool empty() const;
    bool full() const;
    size_t free() const;
    size_t used() const;

    bool write(const char data);
    uint32_t read(bool *success = NULL);

private:
    // Private data
    Utf8 m_utf8;
    uint32_t *m_buffer;
    size_t m_bufferSize;
    size_t m_writePosition;
    size_t m_readPosition;
};
}

#endif // MINISAXCPP_UNICODECIRCULARBUFFER_H
