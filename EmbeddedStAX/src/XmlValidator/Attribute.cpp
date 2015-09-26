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

#include <EmbeddedStAX/XmlValidator/Attribute.h>
#include <EmbeddedStAX/XmlValidator/Reference.h>

using namespace EmbeddedStAX;

/**
 * Validate an Attribute Value
 *
 * \param attributeValue    UTF-8 encoded string
 * \param quotationMark     Quotation mark
 *
 * \retval true     Valid
 * \retval false    Invalid
 *
 * Format:
 * \code{.unparsed}
 * AttValue ::= '"' ([^<&"] | Reference)* '"'
 *           |  "'" ([^<&'] | Reference)* "'"
 * \endcode
 */
bool XmlValidator::validateAttributeValue(const std::string &attributeValue,
                                          const Common::QuotationMark quotationMark)
{
    bool valid = true;

    if (!attributeValue.empty())
    {
        valid = validateAttributeValue(Common::Utf8::toUnicodeString(attributeValue),
                                       quotationMark);
    }

    return valid;
}

/**
 * Validate an Attribute Value
 *
 * \param attributeValue    Unicode string
 * \param quotationMark     Quotation mark
 *
 * \retval true     Valid
 * \retval false    Invalid
 *
 * Format:
 * \code{.unparsed}
 * AttValue	::= '"' ([^<&"] | Reference)* '"'
             |  "'" ([^<&'] | Reference)* "'"
 * \endcode
 */
bool XmlValidator::validateAttributeValue(const Common::UnicodeString &attributeValue,
                                          const Common::QuotationMark quotationMark)
{
    bool valid = true;

    if (!attributeValue.empty())
    {
        bool validationFinished = false;
        size_t position = 0U;

        while ((position < attributeValue.size()) && !validationFinished)
        {
            // Check if valid attribute value character
            switch (attributeValue.at(position))
            {
                case static_cast<uint32_t>('^'):
                case static_cast<uint32_t>('<'):
                {
                    // Error, invalid character
                    valid = false;
                    validationFinished = true;
                    break;
                }

                case static_cast<uint32_t>('"'):
                {
                    if (quotationMark == Common::QuotationMark_Quote)
                    {
                        // Error, invalid character
                        valid = false;
                        validationFinished = true;
                    }
                    break;
                }

                case static_cast<uint32_t>('\''):
                {
                    if (quotationMark == Common::QuotationMark_Apostrophe)
                    {
                        // Error, invalid character
                        valid = false;
                        validationFinished = true;
                    }
                    break;
                }

                case static_cast<uint32_t>('&'):
                {
                    // Check for a reference
                    size_t nextPosition = 0U;

                    if (validateReferece(attributeValue, position, &nextPosition))
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
