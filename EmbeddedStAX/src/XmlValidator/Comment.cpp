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

#include <EmbeddedStAX/XmlValidator/Comment.h>

using namespace EmbeddedStAX;

/**
 * Validate a Comment Text
 *
 * \param commentText   UTF-8 encoded Comment Text
 *
 * \retval true     Valid
 * \retval false    Invalid
 *
 * Format:
 * \code{.unparsed}
 * Comment Text ::= ((Char - '-') | ('-' (Char - '-')))*
 * \endcode
 */
bool XmlValidator::validateCommentText(const std::string &commentText)
{
    bool valid = true;

    if (!commentText.empty())
    {
        valid = validateCommentText(Common::Utf8::toUnicodeString(commentText));
    }

    return valid;
}

/**
 * Validate a Comment Text
 *
 * \param commentText   Comment Text (unicode) string
 *
 * \retval true     Valid
 * \retval false    Invalid
 *
 * Format:
 * \code{.unparsed}
 * Comment Text ::= ((Char - '-') | ('-' (Char - '-')))*
 * \endcode
 */
bool XmlValidator::validateCommentText(const Common::UnicodeString &commentText)
{
    bool valid = true;

    if (!commentText.empty())
    {
        const uint32_t minusChar = static_cast<uint32_t>('-');

        // Must not be a single '-' character
        if (commentText.size() == 1U)
        {
            if (commentText.at(0U) == minusChar)
            {
                valid = false;
            }
        }
        // Must not contain "--" sequence
        else
        {
            for (size_t i = 1U; i < commentText.size(); i++)
            {
                if ((commentText.at(i - 1U) == minusChar) &&
                    (commentText.at(i) == minusChar))
                {
                    valid = false;
                }
            }
        }
    }

    return valid;
}
