#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomAiFederatedRound.cmake
#
# PURPOSE:
#   Register an additive C23 weighted-delta aggregation component and
#   bounded native regression tests.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
get_filename_component(_umicom_fed_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
if(NOT TARGET umicom_ai_federated_round)
    add_library(umicom_ai_federated_round STATIC
        "${_umicom_fed_root}/src/ai/training/federated_round.c")
    add_library(Umicom::AiFederatedRound ALIAS umicom_ai_federated_round)
    target_include_directories(umicom_ai_federated_round PUBLIC
        "${_umicom_fed_root}/include")
    target_compile_features(umicom_ai_federated_round PUBLIC c_std_23)
    if(MSVC)
        target_compile_options(umicom_ai_federated_round PRIVATE /W4)
    else()
        target_compile_options(umicom_ai_federated_round PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
        target_link_libraries(umicom_ai_federated_round PUBLIC m)
    endif()
endif()
if(BUILD_TESTING)
    foreach(_case IN ITEMS weighted rejected lora)
        set(_target "umicom-ai-federated-round-${_case}-test")
        if(NOT TARGET "${_target}")
            add_executable("${_target}"
                "${_umicom_fed_root}/tests/ai_training/test_federated_round_${_case}.c")
            target_link_libraries("${_target}" PRIVATE Umicom::AiFederatedRound)
            target_compile_features("${_target}" PRIVATE c_std_23)
            if(MSVC)
                target_compile_options("${_target}" PRIVATE /W4)
            else()
                target_compile_options("${_target}" PRIVATE
                    -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
            endif()
            add_test(NAME "framework.ai.federated_round.${_case}"
                COMMAND "${_target}")
            set_tests_properties("framework.ai.federated_round.${_case}"
                PROPERTIES LABELS "framework;ai;federated;training" TIMEOUT 30)
        endif()
    endforeach()
endif()
if(NOT TARGET umicom-native-federated-round-demo)
    add_executable(umicom-native-federated-round-demo
        "${_umicom_fed_root}/learning/native-ai-federated-round/demo.c")
    target_link_libraries(umicom-native-federated-round-demo
        PRIVATE Umicom::AiFederatedRound)
    target_compile_features(umicom-native-federated-round-demo PRIVATE c_std_23)
endif()
