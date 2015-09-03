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
#include "Attribute.h"

namespace MiniSaxCpp
{
/**
 * XML Reader class can be used to parse a XML document
 */
class XmlReader
{
public:
    XmlReader();

    void clear();
    void addData(const std::string &data);
    
    ParsingResult Parse();
    ParsingResult getLastParsingResult();

    std::string getName() const;
    AttributeList getAttributeList() const;
    std::string getTextNode() const;
    std::string getComment() const;
    
public:
    enum ParsingResult
    {
        ParsingResult_None,
        ParsingResult_NeedMoreData,
        ParsingResult_Error,
        ParsingResult_ProcessingInstruction,
        ParsingResult_SelfClosingElement,
        ParsingResult_StartOfElement,
        ParsingResult_EndOfElement,
        ParsingResult_TextNode,
        ParsingResult_Comment
    };

private:
    enum State
    {
        State_Idle,
        
        State_ReadingTagType,
        
        State_ReadingProcessingInformationName,
        State_ReadingProcessingInformationAttributeName,
        State_ReadingProcessingInformationAttributeValue,
        
        State_ReadingStartOfElement,
        State_ReadingElementAttributeName,
        State_ReadingElementAttributeValue,
        State_ReadingTextNode,
        
        State_ReadingEndOfElement,
        
        State_ReadingComment
    };
    
    State m_state;
    std::string m_dataBuffer;
    
    ParsingResult m_lastParsingResult;
    std::string m_name;
    AttributeList m_attributeList;
    std::string m_textNode;
    std::string m_comment;
};
}
