cmake_minimum_required(VERSION 2.6)
project(embeddedstaxdemo)

# EmbeddedStAX (sources and headers)
add_subdirectory(../EmbeddedStAX ${CMAKE_CURRENT_BINARY_DIR}/EmbeddedStAX)
include_directories( ${embeddedstax_INCLUDE} )

# Demo project
set(embeddedstaxdemo_SOURCES
        main.cpp)

add_executable(embeddedstaxdemo ${embeddedstax_SOURCES}
                                ${embeddedstax_HEADERS}
                                ${embeddedstaxdemo_SOURCES})
