cmake_minimum_required(VERSION 2.6)

# Directory: Common
set(embeddedstax_SOURCES_Common
        ${CMAKE_CURRENT_SOURCE_DIR}/Common/Common.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Common/Utf.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Common/XmlValidator.cpp)

set(embeddedstax_HEADERS_Common
        ${CMAKE_CURRENT_SOURCE_DIR}/Common/Common.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Common/Utf.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Common/XmlValidator.h)

# Directory: XmlReader
set(embeddedstax_SOURCES_XmlReader
        ${CMAKE_CURRENT_SOURCE_DIR}/XmlReader/ParsingBuffer.cpp

        ${CMAKE_CURRENT_SOURCE_DIR}/XmlReader/XmlItemParser.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/XmlReader/XmlReader.cpp)

set(embeddedstax_HEADERS_XmlReader
        ${CMAKE_CURRENT_SOURCE_DIR}/XmlReader/ParsingBuffer.h

        ${CMAKE_CURRENT_SOURCE_DIR}/XmlReader/XmlItemParser.h
        ${CMAKE_CURRENT_SOURCE_DIR}/XmlReader/XmlReader.h)

# Directory: XmlReader/Tokens
set(embeddedstax_SOURCES_XmlReader_Tokens
        ${CMAKE_CURRENT_SOURCE_DIR}/XmlReader/Tokens/AbstractToken.cpp)

set(embeddedstax_HEADERS_XmlReader_Tokens
        ${CMAKE_CURRENT_SOURCE_DIR}/XmlReader/Tokens/AbstractToken.h)

# Directory: XmlWriter
set(embeddedstax_SOURCES_XmlWriter
        ${CMAKE_CURRENT_SOURCE_DIR}/XmlWriter/XmlWriter.cpp)

set(embeddedstax_HEADERS_XmlWriter
        ${CMAKE_CURRENT_SOURCE_DIR}/XmlWriter/XmlWriter.h)

# Group all
set(embeddedstax_SOURCES
        ${embeddedstax_SOURCES_Common}
        ${embeddedstax_SOURCES_XmlReader}
        ${embeddedstax_SOURCES_XmlWriter}
        ${embeddedstax_SOURCES_XmlReader_Tokens}
        PARENT_SCOPE)

set(embeddedstax_HEADERS
        ${embeddedstax_HEADERS_Common}
        ${embeddedstax_HEADERS_XmlReader}
        ${embeddedstax_HEADERS_XmlWriter}
        ${embeddedstax_HEADERS_XmlReader_Tokens}
        PARENT_SCOPE)

# Inclide directories
set(embeddedstax_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
