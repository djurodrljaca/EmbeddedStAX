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
#include <string>
#include "Common.h"

namespace MiniSaxCpp
{
namespace XmlValidator
{
bool validateNameString(const std::string &name);
bool validateCommentString(const std::string &commentText);
bool validatePiTargetString(const std::string &piTarget);
bool validatePiValueString(const std::string &piValue);
bool validateAttValueString(const std::string &attValue, const Common::QuotationMark quotationMark);
bool validateTextNodeString(const std::string &textNode);

// TODO: move these functions to source file?
bool validateCharRefString(const std::string &str,
                           const size_t startPosition,
                           size_t *nextCharacterPosition = NULL);
bool validateEntityRefString(const std::string &str,
                             const size_t startPosition,
                             size_t *nextCharacterPosition = NULL);
}
}
