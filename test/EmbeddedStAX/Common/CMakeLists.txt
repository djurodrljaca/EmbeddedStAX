cmake_minimum_required(VERSION 2.6)

# Unit tests
set(testembeddedstax_EmbeddedStAX_Common_SOURCES
        ${embeddedstax_EmbeddedStAX_src_PATH}/Common/Common.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Common_unittest.cpp
        PARENT_SCOPE
    )

set(testembeddedstax_EmbeddedStAX_Common_HEADERS
        #${embeddedstax_EmbeddedStAX_inc_PATH}/EmbeddedStAX/Common/Common.h
        PARENT_SCOPE
    )
