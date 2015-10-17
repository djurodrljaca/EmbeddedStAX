cmake_minimum_required(VERSION 2.6)

# EmbeddedStAX
set(embeddedstax_PATH ../../EmbeddedStAX)
add_subdirectory(${embeddedstax_PATH} ${CMAKE_CURRENT_BINARY_DIR}/EmbeddedStAX)

set(testembeddedstax_EmbeddedStAX_INCLUDES
        ${embeddedstax_INCLUDE}
        PARENT_SCOPE
    )


get_filename_component(embeddedstax_EmbeddedStAX_src_PATH
                       ${CMAKE_CURRENT_SOURCE_DIR}/../${embeddedstax_PATH}/EmbeddedStAX/src
                       ABSOLUTE)
get_filename_component(embeddedstax_EmbeddedStAX_inc_PATH
                       ${CMAKE_CURRENT_SOURCE_DIR}/../${embeddedstax_PATH}/EmbeddedStAX/inc
                       ABSOLUTE)

# Unit tests
add_subdirectory(Common)

set(testembeddedstax_EmbeddedStAX_SOURCES
        ${testembeddedstax_EmbeddedStAX_Common_SOURCES}
        PARENT_SCOPE
    )

set(testembeddedstax_EmbeddedStAX_HEADERS
        ${testembeddedstax_EmbeddedStAX_Common_HEADERS}
        PARENT_SCOPE
    )
