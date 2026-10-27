#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomAiStackedKvPool.cmake
#
# PURPOSE:
#   Register the native FP32 stacked K/V lease pool and its regression tests.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
get_filename_component(_umicom_pool_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
if(NOT TARGET Umicom::AiInferenceMemoryPlan)
    include("${CMAKE_CURRENT_LIST_DIR}/UmicomAiInferenceMemoryPlan.cmake")
endif()
if(NOT TARGET Umicom::AiStackedKvStore)
    include("${CMAKE_CURRENT_LIST_DIR}/UmicomAiStackedKvStore.cmake")
endif()
if(NOT TARGET umicom_ai_stacked_kv_pool)
    add_library(umicom_ai_stacked_kv_pool STATIC
        "${_umicom_pool_root}/src/ai/training/stacked_kv_pool.c")
    add_library(Umicom::AiStackedKvPool ALIAS umicom_ai_stacked_kv_pool)
    target_include_directories(umicom_ai_stacked_kv_pool PUBLIC
        "${_umicom_pool_root}/include")
    target_compile_features(umicom_ai_stacked_kv_pool PUBLIC c_std_23)
    target_link_libraries(umicom_ai_stacked_kv_pool PUBLIC
        Umicom::AiInferenceMemoryPlan Umicom::AiStackedKvStore)
    if(MSVC)
        target_compile_options(umicom_ai_stacked_kv_pool PRIVATE /W4)
    else()
        target_compile_options(umicom_ai_stacked_kv_pool PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
    endif()
endif()
if(BUILD_TESTING)
    foreach(_case IN ITEMS lifecycle budget rollover rejections idle_reuse idle_trim idle_budget idle_stale)
        set(_test "umicom-ai-stacked-kv-pool-${_case}-test")
        if(NOT TARGET "${_test}")
            add_executable("${_test}"
                "${_umicom_pool_root}/tests/ai_training/test_stacked_kv_pool_${_case}.c")
            target_link_libraries("${_test}" PRIVATE Umicom::AiStackedKvPool)
            target_compile_features("${_test}" PRIVATE c_std_23)
            if(MSVC)
                target_compile_options("${_test}" PRIVATE /W4)
            else()
                target_compile_options("${_test}" PRIVATE
                    -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
            endif()
            add_test(NAME "framework.ai.stacked_kv_pool.${_case}"
                COMMAND "${_test}")
            set_tests_properties("framework.ai.stacked_kv_pool.${_case}"
                PROPERTIES LABELS "framework;ai;kv;ownership" TIMEOUT 120)
        endif()
    endforeach()
endif()
if(NOT TARGET umicom-native-stacked-kv-pool-demo)
    add_executable(umicom-native-stacked-kv-pool-demo
        "${_umicom_pool_root}/learning/native-ai-kv-session-pool/demo.c")
    target_link_libraries(umicom-native-stacked-kv-pool-demo PRIVATE
        Umicom::AiStackedKvPool)
    target_compile_features(umicom-native-stacked-kv-pool-demo PRIVATE c_std_23)
endif()
