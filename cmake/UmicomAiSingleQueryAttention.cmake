#-----------------------------------------------------------------------------
# Umicom Framework | Opt-in native single-query attention C23 forward kernel
# AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
get_filename_component(UMICOM_AI_SINGLE_QUERY_ROOT
    "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)

if(NOT TARGET umicom_ai_single_query_attention)
    add_library(umicom_ai_single_query_attention STATIC
        "${UMICOM_AI_SINGLE_QUERY_ROOT}/src/ai/training/single_query_attention.c")
    add_library(Umicom::AiSingleQueryAttention ALIAS
        umicom_ai_single_query_attention)
    target_include_directories(umicom_ai_single_query_attention PUBLIC
        "${UMICOM_AI_SINGLE_QUERY_ROOT}/include")
    target_compile_features(umicom_ai_single_query_attention PUBLIC c_std_23)
    if(NOT MSVC)
        target_compile_options(umicom_ai_single_query_attention PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
        if(UNIX)
            target_link_libraries(umicom_ai_single_query_attention PUBLIC m)
        endif()
    else()
        target_compile_options(umicom_ai_single_query_attention PRIVATE /W4)
    endif()
endif()

if(BUILD_TESTING)
    if(NOT TARGET umicom-ai-single-query-unit-test)
        add_executable(umicom-ai-single-query-unit-test
            "${UMICOM_AI_SINGLE_QUERY_ROOT}/tests/ai_training/test_single_query_attention.c")
        target_link_libraries(umicom-ai-single-query-unit-test PRIVATE
            Umicom::AiSingleQueryAttention)
        add_test(NAME framework.ai.single_query.unit
            COMMAND umicom-ai-single-query-unit-test)
        set_tests_properties(framework.ai.single_query.unit PROPERTIES
            LABELS "framework;ai;attention;single-query" TIMEOUT 60)
    endif()
    # An optional integration comparison reads the CURRENT canonical tape.
    # It is registered only when the existing stacked prefix dependency
    # target is already available in the including CMake project.
    if(TARGET Umicom::AiStackedPrefixInference AND
       NOT TARGET umicom-ai-single-query-tape-parity-test)
        add_executable(umicom-ai-single-query-tape-parity-test
            "${UMICOM_AI_SINGLE_QUERY_ROOT}/tests/ai_training/test_single_query_tape_parity.c")
        target_link_libraries(umicom-ai-single-query-tape-parity-test PRIVATE
            Umicom::AiSingleQueryAttention Umicom::AiStackedPrefixInference)
        add_test(NAME framework.ai.single_query.tape_parity
            COMMAND umicom-ai-single-query-tape-parity-test)
        set_tests_properties(framework.ai.single_query.tape_parity PROPERTIES
            LABELS "framework;ai;attention;single-query;parity" TIMEOUT 60)
    endif()
endif()
