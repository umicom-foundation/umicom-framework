#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomAiStackedKvScheduler.cmake
#
# PURPOSE:
#   Register the native cooperative K/V admission and step scheduler.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
get_filename_component(_umicom_kv_scheduler_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
if(NOT TARGET Umicom::AiStackedKvPool)
    include("${CMAKE_CURRENT_LIST_DIR}/UmicomAiStackedKvPool.cmake")
endif()
if(NOT TARGET umicom_ai_stacked_kv_scheduler)
    add_library(umicom_ai_stacked_kv_scheduler STATIC
        "${_umicom_kv_scheduler_root}/src/ai/training/stacked_kv_scheduler.c")
    add_library(Umicom::AiStackedKvScheduler ALIAS umicom_ai_stacked_kv_scheduler)
    target_include_directories(umicom_ai_stacked_kv_scheduler PUBLIC
        "${_umicom_kv_scheduler_root}/include")
    target_link_libraries(umicom_ai_stacked_kv_scheduler PUBLIC
        Umicom::AiStackedKvPool)
    target_compile_features(umicom_ai_stacked_kv_scheduler PUBLIC c_std_23)
    if(MSVC)
        target_compile_options(umicom_ai_stacked_kv_scheduler PRIVATE /W4)
    else()
        target_compile_options(umicom_ai_stacked_kv_scheduler PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
    endif()
endif()
if(BUILD_TESTING)
    foreach(_case IN ITEMS fairness admission cancellation boundaries)
        set(_test "umicom-ai-stacked-kv-scheduler-${_case}-test")
        if(NOT TARGET "${_test}")
            add_executable("${_test}"
                "${_umicom_kv_scheduler_root}/tests/ai_training/test_stacked_kv_scheduler_${_case}.c")
            target_link_libraries("${_test}" PRIVATE
                Umicom::AiStackedKvScheduler)
            target_compile_features("${_test}" PRIVATE c_std_23)
            if(MSVC)
                target_compile_options("${_test}" PRIVATE /W4)
            else()
                target_compile_options("${_test}" PRIVATE
                    -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
            endif()
            add_test(NAME "framework.ai.stacked_kv_scheduler.${_case}"
                COMMAND "${_test}")
            set_tests_properties("framework.ai.stacked_kv_scheduler.${_case}"
                PROPERTIES LABELS "framework;ai;kv;admission;scheduling" TIMEOUT 120)
        endif()
    endforeach()
endif()
if(NOT TARGET umicom-native-kv-scheduler-demo)
    add_executable(umicom-native-kv-scheduler-demo
        "${_umicom_kv_scheduler_root}/learning/native-ai-kv-scheduler/demo.c")
    target_link_libraries(umicom-native-kv-scheduler-demo PRIVATE
        Umicom::AiStackedKvScheduler)
    target_compile_features(umicom-native-kv-scheduler-demo PRIVATE c_std_23)
endif()
