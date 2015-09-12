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

#include "UnicodeCircularBuffer.h"
#include <inttypes.h>
#include <stdio.h>

using namespace MiniSaxCpp;

/**
 * Constructor for UnicodeCharacterCircularBuffer
 *
 * \param size  Buffer's size (number of unicode characters)
 */
UnicodeCircularBuffer::UnicodeCircularBuffer(const size_t size)
    : m_buffer(new uint32_t[size + 1U]),
      m_bufferSize(size + 1U)
{
    clear();
}

/**
 * Destructor for UnicodeCharacterCircularBuffer
 */
UnicodeCircularBuffer::~UnicodeCircularBuffer()
{
    if (m_buffer != NULL)
    {
        delete[] m_buffer;
    }
}

/**
 * Clear the buffer
 */
void UnicodeCircularBuffer::clear()
{
    m_utf8.clear();
    m_writePosition = 0U;
    m_readPosition = 0U;
}

/**
 * Checks if buffer is empty
 *
 * \retval true     Empty
 * \retval false    Not empty
 */
bool UnicodeCircularBuffer::empty() const
{
    bool emptyFlag = false;

    if (m_bufferSize < 2U)
    {
        // If buffer size is too small (minimum size: 2 unicode characters) then the buffer cannot
        // accept any items - buffer is always full
    }
    else
    {
        // If read and write pointers point to the same position then the buffer is empty
        if (m_writePosition == m_readPosition)
        {
            emptyFlag = true;
        }
    }

    return emptyFlag;
}

/**
 * Checks if buffer is full
 *
 * \retval true     Full
 * \retval false    Not full
 */
bool UnicodeCircularBuffer::full() const
{
    bool fullFlag = false;

    if (m_bufferSize < 2U)
    {
        // If buffer size is too small (minimum size: 2 unicode characters) then the buffer cannot
        // accept any items - buffer is always full
        fullFlag = true;
    }
    else
    {
        // If read position is bigger than the write buffer by one item then the buffer is full
        if (m_readPosition == 0U)
        {
            if (m_writePosition == (m_bufferSize - 1U))
            {
                // Read position is at the start and write position is at the end of the buffer -
                // buffer is full
                fullFlag = true;
            }
        }
        else if ((m_readPosition - 1U) == m_writePosition)
        {
            // Read position is bigger than the write buffer by one item - buffer is full
            fullFlag = true;
        }
    }

    return fullFlag;
}

/**
 * Checks how many free items are in the buffer
 *
 * \return Number of free items
 */
size_t UnicodeCircularBuffer::free() const
{
    size_t freeBytes;

    if (m_bufferSize < 2U)
    {
        // If buffer size is too small (minimum size: 2 unicode characters) then the buffer cannot
        // accept any items - buffer is always full
        freeBytes = 0U;
    }
    else
    {
        if (m_readPosition > m_writePosition)
        {
            freeBytes = m_readPosition - m_writePosition - 1U;
        }
        else
        {
            freeBytes = (m_bufferSize - 1U - m_writePosition) + m_readPosition;
        }
    }

    return freeBytes;
}

/**
 * Checks how many items are used in the buffer
 *
 * \return Number of used items
 */
size_t UnicodeCircularBuffer::used() const
{
    size_t usedBytes;

    if (m_bufferSize < 2U)
    {
        // If buffer size is too small (minimum size: 2 unicode characters) then the buffer cannot
        // accept any items - buffer is always full
        usedBytes = 0U;
    }
    else
    {
        if (m_writePosition >= m_readPosition)
        {
            usedBytes = m_writePosition - m_readPosition;
        }
        else
        {
            usedBytes = (m_bufferSize - 1U - m_readPosition) + m_writePosition;
        }
    }

    return usedBytes;
}

/**
 * Write data to the buffer
 *
 * \param data  Data to write
 *
 * \retval true     Success
 * \retval false    Error, invalid unicode character or buffer is full
 */
bool UnicodeCircularBuffer::write(const char data)
{
    bool success = false;

    if (!full())
    {
        const Utf8::Result utfResult = m_utf8.write(data);

        switch (utfResult)
        {
            case Utf8::Result_Success:
            {
                m_buffer[m_writePosition] = m_utf8.getUnicodeCharacter();
                m_writePosition++;

                if (m_writePosition >= m_bufferSize)
                {
                    // Overflow of the write position
                    m_writePosition = 0U;
                }

                success = true;
                break;
            }

            case Utf8::Result_Incomplete:
            {
                // More data is needed to get a complete unicode character
                success = true;
                break;
            }

            default:
            {
                // Error, invalid unicode character
                break;
            }
        }
    }

    return success;
}

/**
 * Read a unicode character from the buffer
 *
 * \param[out] success  Optional output parameter for signaling success or failure
 *
 * \return Oldest unicode character from the buffer
 */
uint32_t UnicodeCircularBuffer::read(bool *success)
{
    bool dataRead = false;
    uint32_t data = 0U;

    if (!empty())
    {
        data = m_buffer[m_readPosition];
        m_readPosition++;
        dataRead = true;

        if (m_readPosition >= m_bufferSize)
        {
            // Overflow of the read position
            m_readPosition = 0U;
        }
    }

    if (success != NULL)
    {
        *success = dataRead;
    }

    return data;
}
