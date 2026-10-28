#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomAiFederatedAuditChain.cmake
#
# PURPOSE:
#   Register the opt-in C23 federated audit adapter atop the canonical
#   record-chain target and its tamper-evidence regression tests.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
get_filename_component(_umicom_federated_audit_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
if(NOT TARGET Umicom::record_chain)
    message(FATAL_ERROR "Federated audit requires the existing Umicom::record_chain target")
endif()
if(NOT TARGET umicom_ai_federated_audit_chain)
    add_library(umicom_ai_federated_audit_chain STATIC
        "${_umicom_federated_audit_root}/src/ai/training/federated_audit_chain.c")
    add_library(Umicom::AiFederatedAuditChain ALIAS umicom_ai_federated_audit_chain)
    target_include_directories(umicom_ai_federated_audit_chain PUBLIC
        "${_umicom_federated_audit_root}/include")
    target_link_libraries(umicom_ai_federated_audit_chain PUBLIC Umicom::record_chain)
    target_compile_features(umicom_ai_federated_audit_chain PUBLIC c_std_23)
    if(MSVC)
        target_compile_options(umicom_ai_federated_audit_chain PRIVATE /W4)
    else()
        target_compile_options(umicom_ai_federated_audit_chain PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
    endif()
endif()
if(BUILD_TESTING AND NOT TARGET umicom_ai_federated_audit_chain_test)
    add_executable(umicom_ai_federated_audit_chain_test
        "${_umicom_federated_audit_root}/tests/ai_training/test_federated_audit_chain.c")
    target_link_libraries(umicom_ai_federated_audit_chain_test PRIVATE
        Umicom::AiFederatedAuditChain)
    target_compile_features(umicom_ai_federated_audit_chain_test PRIVATE c_std_23)
    add_test(NAME framework.ai.federated_audit_chain
        COMMAND umicom_ai_federated_audit_chain_test)
    set_tests_properties(framework.ai.federated_audit_chain PROPERTIES
        LABELS "framework;ai;federated;audit;record_chain" TIMEOUT 30)
endif()
