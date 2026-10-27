#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomAiStackedQualification.cmake
# PURPOSE: Build opt-in all-logit and K/V numerical qualification using the
#          existing stacked decoder, real KV store and incremental proposer.
# AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
get_filename_component(_umicom_qualification_root
    "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
if(NOT TARGET Umicom::AiInferenceParity)
    include("${CMAKE_CURRENT_LIST_DIR}/UmicomAiInferenceParity.cmake")
endif()
if(NOT TARGET Umicom::AiStackedIncremental)
    include("${CMAKE_CURRENT_LIST_DIR}/UmicomAiStackedIncremental.cmake")
endif()
if(NOT TARGET umicom_ai_stacked_qualification)
    add_library(umicom_ai_stacked_qualification STATIC
        "${_umicom_qualification_root}/src/ai/training/stacked_inference_qualification.c")
    add_library(Umicom::AiStackedQualification ALIAS
        umicom_ai_stacked_qualification)
    target_include_directories(umicom_ai_stacked_qualification PUBLIC
        "${_umicom_qualification_root}/include")
    target_compile_features(umicom_ai_stacked_qualification PUBLIC c_std_23)
    target_link_libraries(umicom_ai_stacked_qualification PUBLIC
        Umicom::AiInferenceParity Umicom::AiStackedIncremental)
    if(MSVC)
        target_compile_options(umicom_ai_stacked_qualification PRIVATE /W4)
    else()
        target_compile_options(umicom_ai_stacked_qualification PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
    endif()
endif()
if(BUILD_TESTING)
    foreach(_kind IN ITEMS qualification qualification_rejections)
        set(_target "umicom-ai-stacked-${_kind}-test")
        if(NOT TARGET "${_target}")
            add_executable("${_target}"
                "${_umicom_qualification_root}/tests/ai_training/test_stacked_inference_${_kind}.c")
            target_link_libraries("${_target}" PRIVATE
                Umicom::AiStackedQualification)
            target_compile_features("${_target}" PRIVATE c_std_23)
            if(NOT MSVC)
                target_compile_options("${_target}" PRIVATE
                    -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
            endif()
            add_test(NAME "framework.ai.stacked_qualification.${_kind}"
                COMMAND "${_target}")
            set_tests_properties("framework.ai.stacked_qualification.${_kind}"
                PROPERTIES LABELS "framework;ai;stacked;qualification" TIMEOUT 180)
        endif()
    endforeach()
endif()
if(NOT TARGET umicom-native-stacked-qualification)
    add_executable(umicom-native-stacked-qualification
        "${_umicom_qualification_root}/learning/native-ai-stacked-qualification/main.c")
    target_link_libraries(umicom-native-stacked-qualification PRIVATE
        Umicom::AiStackedQualification)
    target_compile_features(umicom-native-stacked-qualification PRIVATE c_std_23)
endif()
