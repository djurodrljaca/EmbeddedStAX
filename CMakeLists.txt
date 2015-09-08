cmake_minimum_required(VERSION 2.6)
project(minisaxcpp)

set(minisaxcpp_SOURCES
        Common.cpp
        main.cpp
        Utf.cpp
        XmlItemParser.cpp
        XmlReader.cpp
        XmlValidator.cpp
        XmlWriter.cpp)
set(minisaxcpp_HEADERS
        Common.h
        Utf.h
        XmlItemParser.h
        XmlReader.h
        XmlValidator.h
        XmlWriter.h)

add_executable(minisaxcpp ${minisaxcpp_SOURCES} ${minisaxcpp_HEADERS})

install(TARGETS minisaxcpp RUNTIME DESTINATION bin)
