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
#include <iostream>
#include "XmlWriter.h"

int main(int argc, char **argv)
{
	MiniSaxCpp::XmlWriter xmlWriter;
	
// 	MiniSaxCpp::AttributeList attributeList;
// 	attributeList.push_back(MiniSaxCpp::Attribute("version", "1.0"));
// 	attributeList.push_back(MiniSaxCpp::Attribute("encoding", "UTF-8"));
// 	
// 	bool success = xmlWriter.addProcessingInstruction("xml", attributeList);
// 	
// 	if (success)
// 	{
// 		attributeList.clear();
// 		attributeList.push_back(MiniSaxCpp::Attribute("attr1", "val1"));
// 		attributeList.push_back(MiniSaxCpp::Attribute("attr2", "val2"));
// 		success = xmlWriter.startElement("root", attributeList);
// 	}
// 	
// 	if (success)
// 	{
// 		xmlWriter.addComment(" Comment ");
// 		success = xmlWriter.addSelfClosingElement("child1", attributeList);
// 	}
// 	
// 	if (success)
// 	{
// 		success = xmlWriter.startElement("child2");
// 	}
// 	
// 	if (success)
// 	{
// 		xmlWriter.addTextNode("text");
// 		success = xmlWriter.endElement("child2");
// 	}
// 	
// 	if (success)
// 	{
// 		success = xmlWriter.endElement("root");
// 	}
// 	
// 	if (success)
// 	{
// 	    std::cout << "XML document:" << std::endl << xmlWriter.getMessage();
// 	}
    return 0;
}
