cmake_minimum_required(VERSION 2.6)

set(embeddedstax_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/Common/Common.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Common/Utf.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Common/UnicodeCircularBuffer.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Common/XmlValidator.cpp

        ${CMAKE_CURRENT_SOURCE_DIR}/XmlReader/XmlItemParser.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/XmlReader/XmlReader.cpp

        ${CMAKE_CURRENT_SOURCE_DIR}/XmlWriter/XmlWriter.cpp
        PARENT_SCOPE)

set(embeddedstax_HEADERS
        ${CMAKE_CURRENT_SOURCE_DIR}/Common/Common.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Common/Utf.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Common/UnicodeCircularBuffer.h
        ${CMAKE_CURRENT_SOURCE_DIR}/Common/XmlValidator.h

        ${CMAKE_CURRENT_SOURCE_DIR}/XmlReader/XmlItemParser.h
        ${CMAKE_CURRENT_SOURCE_DIR}/XmlReader/XmlReader.h

        ${CMAKE_CURRENT_SOURCE_DIR}/XmlWriter/XmlWriter.h
        PARENT_SCOPE)
