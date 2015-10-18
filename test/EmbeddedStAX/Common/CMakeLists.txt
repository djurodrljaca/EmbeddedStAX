cmake_minimum_required(VERSION 2.6)

# Unit tests
set(testembeddedstax_EmbeddedStAX_Common_SOURCES
        ${embeddedstax_EmbeddedStAX_src_PATH}/Common/Attribute.cpp
        ${embeddedstax_EmbeddedStAX_src_PATH}/Common/Common.cpp
        ${embeddedstax_EmbeddedStAX_src_PATH}/Common/DocumentType.cpp
        ${embeddedstax_EmbeddedStAX_src_PATH}/Common/ProcessingInstruction.cpp
        ${embeddedstax_EmbeddedStAX_src_PATH}/Common/Utf.cpp

        ${embeddedstax_EmbeddedStAX_src_PATH}/XmlValidator/Common.cpp
        ${embeddedstax_EmbeddedStAX_src_PATH}/XmlValidator/Name.cpp
        ${embeddedstax_EmbeddedStAX_src_PATH}/XmlValidator/ProcessingInstruction.cpp

        ${CMAKE_CURRENT_SOURCE_DIR}/Attribute_unittest.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/Common_unittest.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/DocumentType_unittest.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/ProcessingInstruction_unittest.cpp

        PARENT_SCOPE
    )

set(testembeddedstax_EmbeddedStAX_Common_HEADERS
        #${embeddedstax_EmbeddedStAX_inc_PATH}/EmbeddedStAX/Common/Common.h
        PARENT_SCOPE
    )
