#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomUtf8Stream.cmake
# PURPOSE: Reuse strict native C23 UTF-8 validation across AI and text UIs.
# AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
get_filename_component(_umicom_utf8_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
if(NOT TARGET umicom_utf8_stream)
    add_library(umicom_utf8_stream STATIC
        "${_umicom_utf8_root}/src/base/utf8_stream.c")
    add_library(Umicom::Utf8Stream ALIAS umicom_utf8_stream)
    target_include_directories(umicom_utf8_stream PUBLIC
        "${_umicom_utf8_root}/include")
    target_compile_features(umicom_utf8_stream PUBLIC c_std_23)
    if(MSVC)
        target_compile_options(umicom_utf8_stream PRIVATE /W4)
    else()
        target_compile_options(umicom_utf8_stream PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
    endif()
endif()
if(BUILD_TESTING AND NOT TARGET umicom-utf8-stream-test)
    add_executable(umicom-utf8-stream-test
        "${_umicom_utf8_root}/tests/base/test_utf8_stream.c")
    target_link_libraries(umicom-utf8-stream-test PRIVATE Umicom::Utf8Stream)
    add_test(NAME framework.base.utf8_stream COMMAND umicom-utf8-stream-test)
    set_tests_properties(framework.base.utf8_stream PROPERTIES
        LABELS "framework;base;utf8;native-ai")
endif()
