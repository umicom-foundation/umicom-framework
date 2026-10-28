#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomAiFederatedEvidenceAnchor.cmake
#
# PURPOSE:
#   Register additive C23 federated Merkle/record-chain evidence composition.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
get_filename_component(_umicom_evidence_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
if(NOT TARGET Umicom::base)
    message(FATAL_ERROR "Federated evidence anchor requires the existing Umicom::base")
endif()
if(NOT TARGET Umicom::record_chain_merkle)
    include("${CMAKE_CURRENT_LIST_DIR}/UmicomRecordChainMerkle.cmake")
endif()
if(NOT TARGET umicom_ai_federated_evidence_anchor)
    add_library(umicom_ai_federated_evidence_anchor STATIC
        "${_umicom_evidence_root}/src/ai/training/federated_evidence_anchor.c")
    add_library(Umicom::AiFederatedEvidenceAnchor ALIAS
        umicom_ai_federated_evidence_anchor)
    target_include_directories(umicom_ai_federated_evidence_anchor PUBLIC
        "${_umicom_evidence_root}/include")
    target_link_libraries(umicom_ai_federated_evidence_anchor PUBLIC
        Umicom::record_chain_merkle Umicom::record_chain)
    target_compile_features(umicom_ai_federated_evidence_anchor PUBLIC c_std_23)
    if(MSVC)
        target_compile_options(umicom_ai_federated_evidence_anchor PRIVATE /W4)
    else()
        target_compile_options(umicom_ai_federated_evidence_anchor PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
    endif()
endif()
if(BUILD_TESTING AND NOT TARGET umicom-ai-federated-evidence-anchor-test)
    add_executable(umicom-ai-federated-evidence-anchor-test
        "${_umicom_evidence_root}/tests/ai_training/test_federated_evidence_anchor.c")
    target_link_libraries(umicom-ai-federated-evidence-anchor-test PRIVATE
        Umicom::AiFederatedEvidenceAnchor)
    target_compile_features(umicom-ai-federated-evidence-anchor-test PRIVATE c_std_23)
    foreach(_case IN ITEMS happy tamper rejection)
        add_test(NAME "framework.ai.federated_evidence.${_case}"
            COMMAND umicom-ai-federated-evidence-anchor-test "${_case}")
        set_tests_properties("framework.ai.federated_evidence.${_case}"
            PROPERTIES LABELS "framework;ai;federated;merkle;record_chain" TIMEOUT 30)
    endforeach()
endif()
