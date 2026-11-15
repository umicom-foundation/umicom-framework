#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomUtf8Stream.cmake
#
# PURPOSE:
# Integrate incremental UTF-8 into installed Base and qualify legacy AI collection.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
# The earlier standalone target had no implementation and was not included.
# Retain its composition for review. Base now owns framing beside its canonical
# scalar decoder, so installed SDK consumers reuse Umicom::base directly.
if(FALSE)
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

endif()

include_guard(GLOBAL)
get_filename_component(_umicom_utf8_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
target_sources(umicom_base PRIVATE "${_umicom_utf8_root}/src/base/utf8_stream.c")
# Preserve the helper's build-tree target names without a second implementation.
if(NOT TARGET umicom_utf8_stream)
    add_library(umicom_utf8_stream INTERFACE)
    target_link_libraries(umicom_utf8_stream INTERFACE Umicom::base)
    add_library(Umicom::Utf8Stream ALIAS umicom_utf8_stream)
endif()
if(BUILD_TESTING)
    add_executable(umicom-utf8-stream-test "${_umicom_utf8_root}/tests/base/test_utf8_stream.c")
    add_executable(umicom-utf8-stream-scalars-test "${_umicom_utf8_root}/tests/base/test_utf8_stream_scalars.c")
    foreach(_target IN ITEMS umicom-utf8-stream-test umicom-utf8-stream-scalars-test)
        target_link_libraries(${_target} PRIVATE Umicom::base)
        target_compile_features(${_target} PRIVATE c_std_23)
        umicom_apply_warnings(${_target})
        umicom_apply_sanitizers(${_target})
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(${_target})
        endif()
    endforeach()
    add_test(NAME framework.base.utf8_stream COMMAND umicom-utf8-stream-test)
    add_test(NAME framework.base.utf8_stream_scalars COMMAND umicom-utf8-stream-scalars-test)
    set_tests_properties(framework.base.utf8_stream framework.base.utf8_stream_scalars
        PROPERTIES TIMEOUT 120 LABELS "framework;base;utf8;native-ai")
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners(framework.base.utf8_stream TARGETS umicom-utf8-stream-test)
        umicom_test_impact_owners(framework.base.utf8_stream_scalars TARGETS umicom-utf8-stream-scalars-test)
    endif()
    add_executable(umicom-ai-stream-collector-test "${_umicom_utf8_root}/tests/ai/test_stream_collector.c")
    target_link_libraries(umicom-ai-stream-collector-test PRIVATE Umicom::ai)
    target_compile_features(umicom-ai-stream-collector-test PRIVATE c_std_23)
    umicom_apply_warnings(umicom-ai-stream-collector-test)
    umicom_apply_sanitizers(umicom-ai-stream-collector-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-ai-stream-collector-test)
    endif()
    foreach(_case IN ITEMS append empty exact overflow unterminated corrupt-size corrupt-terminator embedded-terminator alias-self alias-slice callback-reset callback-nested final-forwarding arguments)
        add_test(NAME "framework.ai.stream_collector.${_case}"
            COMMAND umicom-ai-stream-collector-test "${_case}")
        set_tests_properties("framework.ai.stream_collector.${_case}" PROPERTIES
            TIMEOUT 30 LABELS "framework;ai;stream;ownership")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("framework.ai.stream_collector.${_case}"
                TARGETS umicom-ai-stream-collector-test)
        endif()
    endforeach()
endif()
