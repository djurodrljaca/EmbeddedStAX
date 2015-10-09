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

#include <EmbeddedStAX/XmlValidator/ProcessingInstruction.h>
#include <EmbeddedStAX/XmlValidator/Common.h>
#include <EmbeddedStAX/XmlValidator/Name.h>

using namespace EmbeddedStAX;

/**
 * Check if PITarget represents a XML declaration
 *
 * \param piTarget  PITarget (unicode) string
 *
 * \retval true     Is a XML declaration
 * \retval false    Is not a XML declaration
 *
 * Format:
 * \code{.unparsed}
 * XML declaration ::= ('X' | 'x') ('M' | 'm') ('L' | 'l')
 * \endcode
 */
bool XmlValidator::isXmlDeclaration(const Common::UnicodeString &piTarget)
{
    bool success = false;

    if (Common::compareUnicodeString(0U, piTarget, std::string("xml"), std::string("XML")))
    {
        success = true;
    }

    return success;
}

/**
 * Validate a PITarget
 *
 * \param piTarget  PITarget (unicode) string
 *
 * \retval true     Valid
 * \retval false    Invalid
 *
 * Format:
 * \code{.unparsed}
 * PITarget ::= Name - (('X' | 'x') ('M' | 'm') ('L' | 'l'))
 * \endcode
 */
bool XmlValidator::validatePiTarget(const Common::UnicodeString &piTarget)
{
    bool valid = false;

    if (!piTarget.empty())
    {
        valid = validateName(piTarget);

        if (valid)
        {
            if (isXmlDeclaration(piTarget))
            {
                valid = false;
            }
        }
    }

    return valid;
}

/**
 * Validate a PITarget
 *
 * \param piTarget  PITarget (unicode) string
 *
 * \retval true     Valid
 * \retval false    Invalid
 *
 * Format:
 * \code{.unparsed}
 * PI Data ::= (Char* - (Char* '?>' Char*))
 * \endcode
 */
bool XmlValidator::validatePiData(const Common::UnicodeString &piData)
{
    bool valid = false;

    for (size_t i = 0U; i < piData.size(); i++)
    {
        valid = isChar(piData.at(i));

        if (valid)
        {
            if (i > 0U)
            {
                if ((piData.at(i - 1U) == static_cast<uint32_t>('?')) &&
                    (piData.at(i) == static_cast<uint32_t>('>')))
                {
                    valid = false;
                }
            }
        }
    }

    return valid;
}
