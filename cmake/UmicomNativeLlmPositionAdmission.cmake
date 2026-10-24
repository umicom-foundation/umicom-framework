#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomNativeLlmPositionAdmission.cmake
# PURPOSE:
#   Register additive checkpoint-bound loading, generation, and benchmark
#   admission in native C23. Neither decoder source nor checkpoint ABI changes.
#   Applications must consume these Framework services, not duplicate them.
# AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
get_filename_component(_umicom_position_admission_root
    "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)

if(NOT TARGET umicom_ai_training)
    message(FATAL_ERROR "Include UmicomNativeLlmTraining.cmake first")
endif()
if(NOT TARGET umicom-native-llm-position-contract)
    message(FATAL_ERROR "Include UmicomNativeLlmPositionContract.cmake first")
endif()
if(NOT TARGET umicom-native-llm-prefix-benchmark)
    message(FATAL_ERROR "Include UmicomNativeLlmPrefixBenchmark.cmake first")
endif()

target_sources(umicom_ai_training PRIVATE
    "${_umicom_position_admission_root}/src/ai/training/position_admission.c")

if(NOT TARGET umicom-native-llm-position-admission)
    add_executable(umicom-native-llm-position-admission
        "${_umicom_position_admission_root}/learning/native-llm/prefix-work/inspect_position_admission.c")
    target_link_libraries(umicom-native-llm-position-admission PRIVATE Umicom::AiTraining)
    target_compile_features(umicom-native-llm-position-admission PRIVATE c_std_23)
    if(MSVC)
        target_compile_options(umicom-native-llm-position-admission PRIVATE /W4)
    else()
        target_compile_options(umicom-native-llm-position-admission PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
    endif()
endif()

if(BUILD_TESTING)
    foreach(_umicom_admission_case IN ITEMS roundtrip rejections identity)
        set(_umicom_admission_target
            "umicom-ai-training-position-admission-${_umicom_admission_case}-test")
        if(NOT TARGET "${_umicom_admission_target}")
            add_executable("${_umicom_admission_target}"
                "${_umicom_position_admission_root}/tests/ai_training/test_training_position_admission_${_umicom_admission_case}.c")
            target_link_libraries("${_umicom_admission_target}" PRIVATE Umicom::AiTraining)
            target_compile_features("${_umicom_admission_target}" PRIVATE c_std_23)
            if(MSVC)
                target_compile_options("${_umicom_admission_target}" PRIVATE /W4)
            else()
                target_compile_options("${_umicom_admission_target}" PRIVATE
                    -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
            endif()
            add_test(NAME
                "framework.ai.training.position_admission_${_umicom_admission_case}"
                COMMAND "${_umicom_admission_target}")
            set_tests_properties(
                "framework.ai.training.position_admission_${_umicom_admission_case}"
                PROPERTIES LABELS "framework;ai;native-training;position;checkpoint;admission")
        endif()
    endforeach()
    add_test(NAME framework.ai.training.position_admission_demo
             COMMAND umicom-native-llm-position-admission)
    set_tests_properties(framework.ai.training.position_admission_demo
        PROPERTIES LABELS "framework;ai;native-training;position;learning;admission")
endif()
