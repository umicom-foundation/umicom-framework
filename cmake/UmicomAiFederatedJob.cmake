#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomAiFederatedJob.cmake
#
# PURPOSE:
#   Register native bounded federated-job admission and its regression suite.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
get_filename_component(_umicom_job_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
if(NOT TARGET Umicom::AiFederatedRound)
    include("${CMAKE_CURRENT_LIST_DIR}/UmicomAiFederatedRound.cmake")
endif()
if(NOT TARGET umicom_ai_federated_job)
    add_library(umicom_ai_federated_job STATIC
        "${_umicom_job_root}/src/ai/training/federated_job.c")
    add_library(Umicom::AiFederatedJob ALIAS umicom_ai_federated_job)
    target_include_directories(umicom_ai_federated_job PUBLIC
        "${_umicom_job_root}/include")
    target_link_libraries(umicom_ai_federated_job PUBLIC Umicom::AiFederatedRound)
    target_compile_features(umicom_ai_federated_job PUBLIC c_std_23)
    if(MSVC)
        target_compile_options(umicom_ai_federated_job PRIVATE /W4)
    else()
        target_compile_options(umicom_ai_federated_job PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
    endif()
endif()
if(BUILD_TESTING)
    foreach(_case IN ITEMS weighted cancel rejected ticket)
        set(_target "umicom-ai-federated-job-${_case}-test")
        if(NOT TARGET "${_target}")
            add_executable("${_target}"
                "${_umicom_job_root}/tests/ai_training/test_federated_job_${_case}.c")
            target_link_libraries("${_target}" PRIVATE Umicom::AiFederatedJob)
            target_compile_features("${_target}" PRIVATE c_std_23)
            if(MSVC)
                target_compile_options("${_target}" PRIVATE /W4)
            else()
                target_compile_options("${_target}" PRIVATE
                    -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
            endif()
            add_test(NAME "framework.ai.federated_job.${_case}" COMMAND "${_target}")
            set_tests_properties("framework.ai.federated_job.${_case}"
                PROPERTIES LABELS "framework;ai;federation;admission" TIMEOUT 45)
        endif()
    endforeach()
endif()
if(NOT TARGET umicom-native-federated-job-demo)
    add_executable(umicom-native-federated-job-demo
        "${_umicom_job_root}/learning/native-ai-federated-job/demo.c")
    target_link_libraries(umicom-native-federated-job-demo PRIVATE Umicom::AiFederatedJob)
    target_compile_features(umicom-native-federated-job-demo PRIVATE c_std_23)
endif()
