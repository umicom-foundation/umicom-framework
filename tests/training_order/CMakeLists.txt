#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/training_order/CMakeLists.txt
#
# PURPOSE:
#   Compose focused checks of the production row permutation and shared RNG.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.25)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    project(UmicomTrainingOrderChecks LANGUAGES C)
    include(CTest)
    get_filename_component(_order_root "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
    add_library(umicom_order_subject STATIC
        "${_order_root}/src/ai/training/sample_order.c"
        "${_order_root}/src/ai/training/initialisation.c")
    target_include_directories(umicom_order_subject PUBLIC "${_order_root}/include")
    target_compile_features(umicom_order_subject PUBLIC c_std_23)
    if(NOT WIN32)
        target_link_libraries(umicom_order_subject PUBLIC m)
    endif()
    if(NOT MSVC)
        target_compile_options(umicom_order_subject PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
    endif()
    set(_order_library umicom_order_subject)
else()
    set(_order_library Umicom::AiTraining)
endif()
foreach(_kind IN ITEMS sample_order order_failures)
    add_executable(umicom-${_kind}-test "test_${_kind}.c")
    target_link_libraries(umicom-${_kind}-test PRIVATE ${_order_library})
    target_compile_features(umicom-${_kind}-test PRIVATE c_std_23)
    if(NOT MSVC)
        target_compile_options(umicom-${_kind}-test PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
    endif()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-${_kind}-test)
    endif()
endforeach()
foreach(_case IN ITEMS vectors permutation maximum repeat single batch-tail arguments capacity)
    add_test(NAME framework.ai.training.sample_order.${_case}
        COMMAND umicom-sample_order-test ${_case})
    set_tests_properties(framework.ai.training.sample_order.${_case} PROPERTIES
        TIMEOUT 30 LABELS "framework;training;dataset;order")
endforeach()
add_test(NAME framework.ai.training.sample_order.failures COMMAND umicom-order_failures-test)
set_tests_properties(framework.ai.training.sample_order.failures PROPERTIES
    TIMEOUT 30 LABELS "framework;training;dataset;ownership")
