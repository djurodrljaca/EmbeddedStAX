cmake_minimum_required(VERSION 2.6)

# Google Test Framework: gmock (also contains gtest)
add_subdirectory(gmock)
include_directories(${gtest_SOURCE_DIR}/include)

# Unit tests
project(testembeddedstax)
add_subdirectory(EmbeddedStAX)
include_directories(${testembeddedstax_EmbeddedStAX_INCLUDES})

set(testembeddedstax_SOURCES
        ${testembeddedstax_EmbeddedStAX_SOURCES}
    )

set(testembeddedstax_HEADERS
        # Add headers here!
    )

add_executable(testembeddedstax ${testembeddedstax_SOURCES}
                                ${testembeddedstax_HEADERS}
    )

target_link_libraries(testembeddedstax gtest_main)
