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

#include <EmbeddedStAX/XmlValidator/Reference.h>
#include <EmbeddedStAX/XmlValidator/TextNode.h>

using namespace EmbeddedStAX;

/**
 * Validate a Text Node
 *
 * \param text  Text (unicode) string
 *
 * \retval true     Valid
 * \retval false    Invalid
 *
 * Format:
 * \code{.unparsed}
 * Text ::= CharData? (Reference CharData?)*
 * \endcode
 */
bool XmlValidator::validateTextNode(const Common::UnicodeString &text)
{
    bool valid = true;

    if (!text.empty())
    {
        bool validationFinished = false;
        size_t position = 0U;

        while ((position < text.size()) && !validationFinished)
        {
            // Check if valid attribute value character
            switch (text.at(position))
            {
                case static_cast<uint32_t>('<'):
                {
                    // Error, invalid character
                    valid = false;
                    validationFinished = true;
                    break;
                }

                case static_cast<uint32_t>('&'):
                {
                    // Check for a reference
                    size_t nextPosition = 0U;

                    if (validateReferece(text, position, &nextPosition))
                    {
                        // Valid reference found
                        position = nextPosition;
                    }
                    else
                    {
                        // Error, invalid reference
                        valid = false;
                        validationFinished = true;
                    }
                    break;
                }

                case static_cast<uint32_t>('>'):
                {
                    // Check if '>' character should have been escaped (if part of ']]>' sequence)
                    if (position >= 2U)
                    {
                        // Check for ']]>' sequence
                        if (Common::compareUnicodeString(position - 2U, text, "]]"))
                        {
                            // Error, invalid character
                            valid = false;
                            validationFinished = true;
                        }
                    }
                    break;
                }

                default:
                {
                    // Valid character
                    position++;
                    break;
                }
            }
        }
    }

    return valid;
}
