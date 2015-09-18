cmake_minimum_required(VERSION 2.6)

# Directory: Common
set(embeddedstax_SOURCES_Common
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Common/Common.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Common/Utf.cpp
    )

set(embeddedstax_HEADERS_Common
        ${CMAKE_CURRENT_SOURCE_DIR}/inc/EmbeddedStAX/Common/Common.h
        ${CMAKE_CURRENT_SOURCE_DIR}/inc/EmbeddedStAX/Common/Utf.h
    )

# Directory: XmlReader
set(embeddedstax_SOURCES_XmlReader
        ${CMAKE_CURRENT_SOURCE_DIR}/src/XmlReader/ParsingBuffer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/XmlReader/XmlItemParser.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/XmlReader/XmlReader.cpp
    )

set(embeddedstax_HEADERS_XmlReader
        ${CMAKE_CURRENT_SOURCE_DIR}/inc/EmbeddedStAX/XmlReader/ParsingBuffer.h
        ${CMAKE_CURRENT_SOURCE_DIR}/inc/EmbeddedStAX/XmlReader/XmlItemParser.h
        ${CMAKE_CURRENT_SOURCE_DIR}/inc/EmbeddedStAX/XmlReader/XmlReader.h
    )

# Directory: XmlReader/TokenParsers
set(embeddedstax_SOURCES_XmlReader_TokenParsers
        ${CMAKE_CURRENT_SOURCE_DIR}/src/XmlReader/TokenParsers/AbstractTokenParser.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/XmlReader/TokenParsers/TokenTypeParser.cpp
    )

set(embeddedstax_HEADERS_XmlReader_TokenParsers
        ${CMAKE_CURRENT_SOURCE_DIR}/inc/EmbeddedStAX/XmlReader/TokenParsers/AbstractTokenParser.h
        ${CMAKE_CURRENT_SOURCE_DIR}/inc/EmbeddedStAX/XmlReader/TokenParsers/TokenTypeParser.h
    )

# Directory: XmlValidator
set(embeddedstax_SOURCES_XmlValidator
        ${CMAKE_CURRENT_SOURCE_DIR}/src/XmlValidator/XmlValidator.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/XmlValidator/Whitespace.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/XmlValidator/Name.cpp
    )

set(embeddedstax_HEADERS_XmlValidator
        ${CMAKE_CURRENT_SOURCE_DIR}/inc/EmbeddedStAX/XmlValidator/XmlValidator.h
        ${CMAKE_CURRENT_SOURCE_DIR}/inc/EmbeddedStAX/XmlValidator/Whitespace.h
        ${CMAKE_CURRENT_SOURCE_DIR}/inc/EmbeddedStAX/XmlValidator/Name.h
    )

# Directory: XmlWriter
set(embeddedstax_SOURCES_XmlWriter
        ${CMAKE_CURRENT_SOURCE_DIR}/src/XmlWriter/XmlWriter.cpp
    )

set(embeddedstax_HEADERS_XmlWriter
        ${CMAKE_CURRENT_SOURCE_DIR}/inc/EmbeddedStAX/XmlWriter/XmlWriter.h
    )

# Group all
set(embeddedstax_SOURCES
        ${embeddedstax_SOURCES_Common}
        ${embeddedstax_SOURCES_XmlReader}
        ${embeddedstax_SOURCES_XmlReader_TokenParsers}
        ${embeddedstax_SOURCES_XmlValidator}
        ${embeddedstax_SOURCES_XmlWriter}
        PARENT_SCOPE
    )

set(embeddedstax_HEADERS
        ${embeddedstax_HEADERS_Common}
        ${embeddedstax_HEADERS_XmlReader}
        ${embeddedstax_HEADERS_XmlReader_TokenParsers}
        ${embeddedstax_HEADERS_XmlValidator}
        ${embeddedstax_HEADERS_XmlWriter}
        PARENT_SCOPE
    )

# Inclide directories
set(embeddedstax_INCLUDE
        ${CMAKE_CURRENT_SOURCE_DIR}/inc
        PARENT_SCOPE
    )
