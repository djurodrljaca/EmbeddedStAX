cmake_minimum_required(VERSION 2.6)
project(minisaxcpp)

set(minisaxcpp_SOURCES
        main.cpp
        Attribute.cpp
        XmlReader.cpp
        XmlValidator.cpp
        XmlWriter.cpp
        Utf.cpp)
set(minisaxcpp_HEADERS
        Attribute.h
        XmlReader.h
        XmlValidator.h
        XmlWriter.h
        Utf.h)

add_executable(minisaxcpp ${minisaxcpp_SOURCES} ${minisaxcpp_HEADERS})

install(TARGETS minisaxcpp RUNTIME DESTINATION bin)
