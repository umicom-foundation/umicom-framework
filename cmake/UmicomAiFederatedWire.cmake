#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomAiFederatedWire.cmake
#
# PURPOSE:
#   Register additive C23 canonical federated messages, existing Framework
#   SHA-256 and federated jobs, and native codec/process-boundary tests.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
get_filename_component(_umicom_wire_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
if(NOT TARGET Umicom::AiFederatedJob)
    include("${CMAKE_CURRENT_LIST_DIR}/UmicomAiFederatedJob.cmake")
endif()

# A standalone learning build uses the very same Framework SHA source; it
# does NOT introduce a second cryptographic implementation. Full Framework
# builds already own that implementation through Umicom::base.
if(TARGET Umicom::base)
    set(_umicom_wire_hash_target Umicom::base)
else()
    if(NOT TARGET umicom_ai_federated_wire_base)
        add_library(umicom_ai_federated_wire_base STATIC
            "${_umicom_wire_root}/src/base/sha256.c")
        target_include_directories(umicom_ai_federated_wire_base PUBLIC
            "${_umicom_wire_root}/include")
        target_compile_features(umicom_ai_federated_wire_base PUBLIC c_std_23)
    endif()
    set(_umicom_wire_hash_target umicom_ai_federated_wire_base)
endif()

if(NOT TARGET umicom_ai_federated_wire)
    add_library(umicom_ai_federated_wire STATIC
        "${_umicom_wire_root}/src/ai/training/federated_wire.c")
    add_library(Umicom::AiFederatedWire ALIAS umicom_ai_federated_wire)
    target_include_directories(umicom_ai_federated_wire PUBLIC
        "${_umicom_wire_root}/include")
    target_link_libraries(umicom_ai_federated_wire PUBLIC
        Umicom::AiFederatedJob ${_umicom_wire_hash_target})
    target_compile_features(umicom_ai_federated_wire PUBLIC c_std_23)
    if(MSVC)
        target_compile_options(umicom_ai_federated_wire PRIVATE /W4)
    else()
        target_compile_options(umicom_ai_federated_wire PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
    endif()
endif()

if(BUILD_TESTING AND NOT TARGET umicom-ai-federated-wire-test)
    add_executable(umicom-ai-federated-wire-test
        "${_umicom_wire_root}/tests/ai_training/test_federated_wire.c")
    target_link_libraries(umicom-ai-federated-wire-test PRIVATE
        Umicom::AiFederatedWire)
    target_compile_features(umicom-ai-federated-wire-test PRIVATE c_std_23)
    if(MSVC)
        target_compile_options(umicom-ai-federated-wire-test PRIVATE /W4)
    else()
        target_compile_options(umicom-ai-federated-wire-test PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
    endif()
    foreach(_case IN ITEMS roundtrip tamper short_capacity invalid_fields
                           finite identity envelope aggregation)
        add_test(NAME "framework.ai.federated_wire.${_case}"
            COMMAND umicom-ai-federated-wire-test "${_case}")
        set_tests_properties("framework.ai.federated_wire.${_case}"
            PROPERTIES LABELS "framework;ai;federated;wire" TIMEOUT 45)
    endforeach()
endif()

if(NOT TARGET umicom-native-federated-wire-demo)
    add_executable(umicom-native-federated-wire-demo
        "${_umicom_wire_root}/learning/native-ai-federated-wire/demo.c")
    target_link_libraries(umicom-native-federated-wire-demo PRIVATE
        Umicom::AiFederatedWire)
    target_compile_features(umicom-native-federated-wire-demo PRIVATE c_std_23)
endif()

# Seven distinct operating-system processes exchange files in the build tree:
# three independent assignment emissions, three worker computations, then a
# governed coordinator aggregation. This is a deterministic LOCAL demonstration
# of a transport-independent wire contract, NOT a network or GPU trainer.
if(BUILD_TESTING)
    file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/federated-wire-process")
    foreach(_worker IN ITEMS 1 2 3)
        set(_dir "${CMAKE_CURRENT_BINARY_DIR}/federated-wire-process")
        add_test(NAME "framework.ai.federated_wire.process.emit_${_worker}"
            COMMAND umicom-native-federated-wire-demo emit "${_worker}"
                "${_dir}/assignment-${_worker}.bin")
        set_tests_properties("framework.ai.federated_wire.process.emit_${_worker}"
            PROPERTIES FIXTURES_SETUP "wire_emit_${_worker}" TIMEOUT 30)
        add_test(NAME "framework.ai.federated_wire.process.worker_${_worker}"
            COMMAND umicom-native-federated-wire-demo worker
                "${_dir}/assignment-${_worker}.bin"
                "${_dir}/result-${_worker}.bin")
        set_tests_properties("framework.ai.federated_wire.process.worker_${_worker}"
            PROPERTIES FIXTURES_REQUIRED "wire_emit_${_worker}"
                       FIXTURES_SETUP "wire_result_${_worker}" TIMEOUT 30)
    endforeach()
    add_test(NAME framework.ai.federated_wire.process.coordinator
        COMMAND umicom-native-federated-wire-demo finish
            "${CMAKE_CURRENT_BINARY_DIR}/federated-wire-process/result-1.bin"
            "${CMAKE_CURRENT_BINARY_DIR}/federated-wire-process/result-2.bin"
            "${CMAKE_CURRENT_BINARY_DIR}/federated-wire-process/result-3.bin")
    set_tests_properties(framework.ai.federated_wire.process.coordinator
        PROPERTIES FIXTURES_REQUIRED "wire_result_1;wire_result_2;wire_result_3"
                   TIMEOUT 45 LABELS "framework;ai;federated;process")
endif()
