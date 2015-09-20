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

#include <EmbeddedStAX/Common/XmlDeclaration.h>
#include <EmbeddedStAX/XmlValidator/Common.h>

using namespace EmbeddedStAX::Common;

/**
 * Constructor
 */
XmlDeclaration::XmlDeclaration(const Version version,
                               const Encoding encoding,
                               const Standalone standalone)
    : m_version(version),
      m_encoding(encoding),
      m_standalone(standalone)
{
}

/**
 * Check if processing instruction is valid
 *
 * \retval true     Valid
 * \retval false    Invalid
 */
bool XmlDeclaration::isValid() const
{
    bool valid = true;

    // Check version
    if (m_version != Version_v1_0)
    {
        valid = false;
    }

    // Check encoding
    if (valid)
    {
        if ((m_encoding != Encoding_None) &&
            (m_encoding != Encoding_Utf8))
        {
            valid = false;
        }
    }

    // Check standalone
    if (valid)
    {
        if ((m_standalone != Standalone_None) &&
            (m_standalone != Standalone_No) &&
            (m_standalone != Standalone_Yes))
        {
            valid = false;
        }
    }

    return valid;
}

/**
 * Clear
 */
void XmlDeclaration::clear()
{
    m_version = Version_None;
    m_encoding = Encoding_None;
    m_standalone = Standalone_None;
}

/**
 * Get version
 *
 * \return Version
 */
XmlDeclaration::Version XmlDeclaration::version() const
{
    return m_version;
}

/**
 * Set version
 *
 * \param version   Version
 */
void XmlDeclaration::setVersion(const XmlDeclaration::Version version)
{
    m_version = version;
}

/**
 * Get encoding
 *
 * \return Encoding
 */
XmlDeclaration::Encoding XmlDeclaration::encoding() const
{
    return m_encoding;
}

/**
 * Set encoding
 *
 * \param encoding  Encoding
 */
void XmlDeclaration::setEncoding(const XmlDeclaration::Encoding encoding)
{
    m_encoding = encoding;
}

/**
 * Get standalone
 *
 * \return Standalone
 */
XmlDeclaration::Standalone XmlDeclaration::standalone() const
{
    return m_standalone;
}

/**
 * Set standalone
 *
 * \param standalone    Standalone
 */
void XmlDeclaration::setStandalone(const XmlDeclaration::Standalone standalone)
{
    m_standalone = standalone;
}

/**
 * Create XML declaration object from PI Data
 *
 * \param piData    PI Data
 *
 * \return XML declaration object
 */
XmlDeclaration XmlDeclaration::fromPiData(const UnicodeString &piData)
{
    XmlDeclaration xmlDeclaration;

    // Skip leading whitespace
    size_t position = skipWhitespace(0U, piData);

    // Find version
    const std::string versionString = "version";

    if (Common::compareUnicodeString(position, piData, versionString))
    {
        // Get value
        position = position + versionString.size();
        const UnicodeString value = extractValue(position, piData, &position);

        // Parse value
        if (Common::compareUnicodeString(0U, value, std::string("1.0")))
        {
            xmlDeclaration.setVersion(Version_v1_0);
        }
    }

    // Find encoding
    position = skipWhitespace(position, piData);
    const std::string encodingString = "encoding";

    if (Common::compareUnicodeString(position, piData, encodingString))
    {
        // Get value
        position = position + encodingString.size();
        const UnicodeString value = extractValue(position, piData, &position);

        // Parse value
        if (Common::compareUnicodeString(0U, value, std::string("UTF-8"), std::string("utf-8")))
        {
            xmlDeclaration.setEncoding(Encoding_Utf8);
        }
        else
        {
            xmlDeclaration.setEncoding(Encoding_Unknown);
        }
    }

    // Find standalone
    position = skipWhitespace(position, piData);
    const std::string standaloneString = "standalone";

    if (Common::compareUnicodeString(position, piData, standaloneString))
    {
        // Get value
        position = position + standaloneString.size();
        const UnicodeString value = extractValue(position, piData, &position);

        // Parse value
        if (Common::compareUnicodeString(0U, value, std::string("yes")))
        {
            xmlDeclaration.setStandalone(Standalone_Yes);
        }
        else if (Common::compareUnicodeString(0U, value, std::string("no")))
        {
            xmlDeclaration.setStandalone(Standalone_No);
        }
        else
        {
            xmlDeclaration.setStandalone(Standalone_Unknown);
        }
    }

    // Check if end of PI Data is valid
    position = skipWhitespace(position, piData);

    if (position < piData.size())
    {
        // Error, invalid character found
        xmlDeclaration.clear();
    }

    return xmlDeclaration;
}

/**
 * Skip whitespace in data
 *
 * \param data          Unicode string
 * \param startPosition Start position
 *
 * \return Position of first character after whitespace characters or size of data if non-whitespace
 *         character was not found
 */
size_t XmlDeclaration::skipWhitespace(const size_t startPosition, const UnicodeString &data)
{
    size_t position = startPosition;

    for (; position < data.size(); position++)
    {
        if (XmlValidator::isWhitespace(data.at(position)) == false)
        {
            // Non-whitespace character found
            break;
        }
    }

    return position;
}

/**
 * Extract value from data
 *
 * \param      startPosition Start position
 * \param      data          Unicode string
 * \param[out] nextPosition  Pointer to position after the value
 *
 * \return Value string
 *
 * Format:
 * \code{.unparsed}
 * S? '=' S? ("'" Value "'" | '"' Value '"')
 * \endcode
 */
UnicodeString XmlDeclaration::extractValue(const size_t startPosition,
                                           const UnicodeString &data,
                                           size_t *nextPosition)
{
    UnicodeString value;

    // Check next position pointer
    bool success = false;

    if (nextPosition != NULL)
    {
        success = true;
    }

    // Check for equal sign
    size_t position = startPosition;

    if (success)
    {
        // Skip whitespace
        position = skipWhitespace(position, data);

        // Check for equal sign
        if (position < data.size())
        {
            if (data.at(position) == static_cast<uint32_t>('='))
            {
                position++;
                success = true;
            }
        }
    }

    // Get quote char
    uint32_t quoteChar = 0U;

    if (success)
    {
        // Skip whitespace
        position = skipWhitespace(position, data);

        // Quote char
        success = false;

        if (position < data.size())
        {
            quoteChar = data.at(position);

            if ((quoteChar == static_cast<uint32_t>('\'')) ||
                (quoteChar == static_cast<uint32_t>('"')))
            {
                position++;
                success = true;
            }
        }
    }

    // Get value
    if (success)
    {
        success = false;

        for (size_t i = position; i < data.size(); i++)
        {
            if (data.at(i) == quoteChar)
            {
                const size_t size = i - position;
                value = data.substr(position, size);

                position = i + 1U;
                success = true;
                break;
            }
        }
    }

    if (success)
    {
        *nextPosition = position;
    }
    else
    {
        value.clear();
    }

    return value;
}
