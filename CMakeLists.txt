cmake_minimum_required(VERSION 2.6)
project(minisaxcpp)

set(minisaxcpp_SOURCES main.cpp Attribute.cpp Attribute.h XmlWriter.cpp XmlWriter.h)

add_executable(minisaxcpp ${minisaxcpp_SOURCES})

install(TARGETS minisaxcpp RUNTIME DESTINATION bin)
