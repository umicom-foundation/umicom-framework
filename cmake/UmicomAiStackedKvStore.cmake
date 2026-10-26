#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomAiStackedKvStore.cmake
# PURPOSE: A reusable native K/V row owner, deliberately separate from
#          inference kernels and trained decoder mathematics.
# AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation | LICENCE: MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
get_filename_component(_umicom_stacked_kv_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)

# The established stacked-prefix planner owns hypothetical performance maths.
# The new store supplies observed storage counters; it never substitutes for
# the Framework decoder's canonical logits/attention calculations.
if(NOT TARGET Umicom::AiStackedPrefixCachePlan)
    include("${CMAKE_CURRENT_LIST_DIR}/UmicomAiStackedPrefixCachePlan.cmake")
endif()
if(NOT TARGET umicom_ai_stacked_kv_store)
    add_library(umicom_ai_stacked_kv_store STATIC
        "${_umicom_stacked_kv_root}/src/ai/training/stacked_kv_store.c"
        "${_umicom_stacked_kv_root}/src/ai/training/stacked_kv_work.c")
    add_library(Umicom::AiStackedKvStore ALIAS umicom_ai_stacked_kv_store)
    target_compile_features(umicom_ai_stacked_kv_store PUBLIC c_std_23)
    target_include_directories(umicom_ai_stacked_kv_store PUBLIC
        "${_umicom_stacked_kv_root}/include")
    target_link_libraries(umicom_ai_stacked_kv_store PUBLIC
        Umicom::AiStackedPrefixCachePlan)
    if(MSVC)
        target_compile_options(umicom_ai_stacked_kv_store PRIVATE /W4)
    else()
        target_compile_options(umicom_ai_stacked_kv_store PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
    endif()
endif()
if(BUILD_TESTING)
    foreach(_case IN ITEMS store rejections work long_run)
        set(_target "umicom-ai-stacked-kv-${_case}-test")
        if(NOT TARGET "${_target}")
            add_executable("${_target}"
                "${_umicom_stacked_kv_root}/tests/ai_training/test_stacked_kv_${_case}.c")
            target_compile_features("${_target}" PRIVATE c_std_23)
            target_link_libraries("${_target}" PRIVATE Umicom::AiStackedKvStore)
            add_test(NAME "framework.ai.stacked_kv.${_case}" COMMAND "${_target}")
            set_tests_properties("framework.ai.stacked_kv.${_case}" PROPERTIES
                LABELS "framework;ai;kv;native-training" TIMEOUT 30)
        endif()
    endforeach()
endif()
if(NOT TARGET umicom-native-stacked-kv-demo)
    add_executable(umicom-native-stacked-kv-demo
        "${_umicom_stacked_kv_root}/learning/native-ai-stacked-kv/demo.c")
    target_link_libraries(umicom-native-stacked-kv-demo PRIVATE Umicom::AiStackedKvStore)
    target_compile_features(umicom-native-stacked-kv-demo PRIVATE c_std_23)
endif()
