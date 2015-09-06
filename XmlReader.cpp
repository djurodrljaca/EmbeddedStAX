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

#include "XmlReader.h"

// using namespace MiniSaxCpp;
//
// /**
//  * Constructor
//  */
// XmlReader::XmlReader()
//     : m_state(State_Idle),
//       m_dataBuffer(),
//       m_lastParsingResult(ParsingResult_None),
//       m_name(),
//       m_attributeList(),
//       m_textNode(),
//       m_comment()
// {
// }
//
// /**
//  * Clear internal state
//  */
// void XmlReader::clear()
// {
//     m_state = State_Idle;
//     m_dataBuffer.clear();
//
//     m_lastParsingResult = ParsingResult_None;
//     m_name.clear();
//     m_attributeList.clear();
//     m_textNode.clear();
//     m_comment.clear();
// }
//
// void XmlReader::addData(const std::string &data)
// {
//     m_dataBuffer.append(data);
// }
//
// XmlReader::ParsingResult XmlReader::Parse()
// {
//     ParsingResult result = ParsingResult_None;
//
//     switch (m_state)
//     {
//         case State_Idle:
//         {
//             m_state = handleStateIdle();
//             break;
//         }
//
//         default:
//         {
//             m_state = State_Idle;
//             break;
//         }
//     }
//
//     return result;
// }
//
// XmlReader::ParsingResult XmlReader::getLastParsingResult()
// {
//     return m_lastParsingResult;
// }
//
// std::string XmlReader::getName() const
// {
//     return m_name;
// }
//
// AttributeList XmlReader::getAttributeList() const
// {
//     return m_attributeList;
// }
//
// std::string XmlReader::getTextNode() const
// {
//     return m_textNode;
// }
//
// std::string XmlReader::getComment() const
// {
//     return m_comment;
// }
//
// XmlReader::State XmlReader::handleStateIdle()
// {
//     State nextState = State_Idle;
//
//     Search for "start of tag" character
//     size_t position = m_dataBuffer.find('<');
//
//     if (position == std::string::npos)
//     {
//         m_dataBuffer.clear();
//     }
//     else
//     {
//         m_dataBuffer.erase(0UL, position + 1UL);
//         m_state = State_ReadingTagType;
//     }
//
//     return nextState;
// }
