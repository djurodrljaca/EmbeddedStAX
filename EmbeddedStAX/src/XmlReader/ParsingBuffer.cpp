#include <EmbeddedStAX/XmlReader/ParsingBuffer.h>

using namespace EmbeddedStAX::XmlReader;

/**
 * Constructor
 */
ParsingBuffer::ParsingBuffer()
    : m_utf8(),
      m_buffer(),
      m_position(0U)
{
}

/**
 * Get the size of the parsing buffer
 *
 * \return Size of the parsing buffer
 */
size_t ParsingBuffer::size() const
{
    return m_buffer.size();
}

/**
 * Clear the buffer
 *
 * \note This will also cleat the utf8 object!
 */
void ParsingBuffer::clear()
{
    m_utf8.clear();
    m_buffer.clear();
    m_position = 0U;
}

/**
 * Erase leading characters
 *
 * \param size  Number of leading characters to erase
 *
 * \note This will also set the current position to the start of the buffer.
 */
void ParsingBuffer::erase(const size_t size)
{
    m_buffer.erase(0U, size);
    m_position = 0U;
}

/**
 * Erase all characters before current position
 *
 * \note This will also set the current position to the start of the buffer.
 */
void ParsingBuffer::eraseToCurrentPosition()
{
    m_buffer.erase(0U, m_position);
    m_position = 0U;
}

/**
 * Get character at the specified position
 *
 * \param position  Position of the requested character
 *
 * \return Character at the specified position.
 * \retval 0 Position is invalid
 */
uint32_t ParsingBuffer::at(const size_t position) const
{
    uint32_t value = 0U;

    if (position < m_buffer.size())
    {
        value = m_buffer.at(position);
    }

    return value;
}

/**
 * Get first character
 *
 * \return First character in the buffer
 * \retval 0 Buffer is empty
 */
uint32_t ParsingBuffer::firstChar() const
{
    uint32_t value = 0U;

    if (!m_buffer.empty())
    {
        value = m_buffer.at(0U);
    }

    return value;
}

/**
 * Get current character
 *
 * \return Current character in the buffer
 * \retval 0 Buffer is empty
 * \retval 0 More data is needed (position is past the end of the buffer)
 */
uint32_t ParsingBuffer::currentChar() const
{
    uint32_t value = 0U;

    if (!m_buffer.empty() && !isMoreDataNeeded())
    {
        value = m_buffer.at(m_position);
    }

    return value;
}

/**
 * Check if more data is needed
 *
 * \retval true     More data is needed
 * \return false    More data is not needed
 *
 * \note More data is needed if position is past the end of the buffer
 */
bool ParsingBuffer::isMoreDataNeeded() const
{
    bool moreDataNeeded = true;

    if (m_position < m_buffer.size())
    {
        moreDataNeeded = false;
    }

    return moreDataNeeded;
}

/**
 * Get current position
 *
 * \return Current position
 */
size_t ParsingBuffer::currentPosition() const
{
    return m_position;
}

/**
 * Set current position
 *
 * \param position  New position
 *
 * \retval true     Position set
 * \retval false    Position not set
 */
bool ParsingBuffer::setCurrentPosition(const size_t position)
{
    bool success = false;

    if (position <= m_buffer.size())
    {
        m_position = position;
        success = true;
    }

    return position;
}

/**
 * Increment current position by one
 *
 * \retval true     Position incremented
 * \retval false    Position not incremented
 */
bool ParsingBuffer::incrementPosition()
{
    bool success = false;

    if (m_position < m_buffer.size())
    {
        m_position++;
        success = true;
    }

    return success;
}

/**
 * Get substring from the buffer
 *
 * \param position  Start position
 * \param size      Number of characters
 *
 * \return Substring
 */
EmbeddedStAX::Common::UnicodeString ParsingBuffer::substring(const size_t position,
                                                             const size_t size) const
{
    Common::UnicodeString data;

    if (position < m_buffer.size())
    {
        data = m_buffer.substr(position, size);
    }

    return data;
}

/**
 * Write data to buffer
 *
 * \param data  UTF-8 encoded string
 *
 * \return Number of character written
 */
size_t ParsingBuffer::writeData(const std::string &data)
{
    size_t charactersWritten = 0U;

    for (size_t i = 0U; i < data.size(); i++)
    {
        const Common::Utf8::Result result = m_utf8.write(data.at(i));

        if (result == Common::Utf8::Result_Success)
        {
            // Unicode character written
            charactersWritten++;
            m_buffer.push_back(m_utf8.getChar());
        }
        else if (result == Common::Utf8::Result_Incomplete)
        {
            // Partial unicode character written
            charactersWritten++;
        }
        else
        {
            // Error, invalid character
            break;
        }
    }

    return charactersWritten;
}
