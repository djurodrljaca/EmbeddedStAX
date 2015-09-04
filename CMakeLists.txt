cmake_minimum_required(VERSION 2.6)
project(minisaxcpp)

set(minisaxcpp_SOURCES
        main.cpp
        Attribute.cpp
        XmlReader.cpp
        XmlWriter.cpp)
set(minisaxcpp_HEADERS
        Attribute.h
        XmlReader.h
        XmlWriter.h)

add_executable(minisaxcpp ${minisaxcpp_SOURCES} ${minisaxcpp_HEADERS})

install(TARGETS minisaxcpp RUNTIME DESTINATION bin)
