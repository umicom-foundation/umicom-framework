#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomAiStackedIncremental.cmake
# PURPOSE: Opt-in, production-tape-owned single-token stacked decoding with
#          retained native K/V and mandatory canonical rollover fallback.
# AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
get_filename_component(UMICOM_AI_INCREMENTAL_ROOT
    "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)

if(NOT TARGET Umicom::AiStackedKvStore)
    include("${CMAKE_CURRENT_LIST_DIR}/UmicomAiStackedKvStore.cmake")
endif()
if(NOT TARGET Umicom::AiStackedPrefixInference)
    include("${CMAKE_CURRENT_LIST_DIR}/UmicomNativeStackedPrefixInference.cmake")
endif()

# The forward-only attention kernel belongs to Framework and is shared with
# its standalone numerical and canonical-tape parity tests.
if(NOT TARGET Umicom::AiSingleQueryAttention)
    include("${CMAKE_CURRENT_LIST_DIR}/UmicomAiSingleQueryAttention.cmake")
endif()

if(NOT TARGET umicom_ai_stacked_incremental)
    add_library(umicom_ai_stacked_incremental STATIC
        "${UMICOM_AI_INCREMENTAL_ROOT}/src/ai/training/stacked_kv_step.c"
        "${UMICOM_AI_INCREMENTAL_ROOT}/src/ai/training/stacked_kv_incremental.c")
    add_library(Umicom::AiStackedIncremental ALIAS
        umicom_ai_stacked_incremental)
    target_include_directories(umicom_ai_stacked_incremental PUBLIC
        "${UMICOM_AI_INCREMENTAL_ROOT}/include")
    target_link_libraries(umicom_ai_stacked_incremental PUBLIC
        Umicom::AiStackedKvStore Umicom::AiStackedPrefixInference
        Umicom::AiSingleQueryAttention)
    target_compile_features(umicom_ai_stacked_incremental PUBLIC c_std_23)
    if(MSVC)
        target_compile_options(umicom_ai_stacked_incremental PRIVATE /W4)
    else()
        target_compile_options(umicom_ai_stacked_incremental PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
    endif()
endif()

if(BUILD_TESTING)
    foreach(test_case IN ITEMS
            step_parity session_parity rollover rejections stale work
            single_query_path)
        set(test_target "umicom-ai-stacked-incremental-${test_case}-test")
        if(NOT TARGET "${test_target}")
            add_executable("${test_target}"
                "${UMICOM_AI_INCREMENTAL_ROOT}/tests/ai_training/test_stacked_incremental_${test_case}.c")
            target_link_libraries("${test_target}" PRIVATE
                Umicom::AiStackedIncremental)
            target_compile_features("${test_target}" PRIVATE c_std_23)
            if(MSVC)
                target_compile_options("${test_target}" PRIVATE /W4)
            else()
                target_compile_options("${test_target}" PRIVATE
                    -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
            endif()
            add_test(NAME "framework.ai.stacked_incremental.${test_case}"
                COMMAND "${test_target}")
            set_tests_properties("framework.ai.stacked_incremental.${test_case}"
                PROPERTIES LABELS "framework;ai;stacked;kv;parity" TIMEOUT 120)
        endif()
    endforeach()
endif()

if(NOT TARGET umicom-native-stacked-incremental-demo)
    add_executable(umicom-native-stacked-incremental-demo
        "${UMICOM_AI_INCREMENTAL_ROOT}/learning/native-ai-stacked-incremental/demo.c")
    target_link_libraries(umicom-native-stacked-incremental-demo PRIVATE
        Umicom::AiStackedIncremental)
    target_compile_features(umicom-native-stacked-incremental-demo PRIVATE c_std_23)
endif()
