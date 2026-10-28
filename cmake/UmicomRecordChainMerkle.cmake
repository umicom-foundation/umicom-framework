#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomRecordChainMerkle.cmake
#
# PURPOSE:
#   Register additive native C23 Merkle membership operations over the
#   canonical Framework SHA and record-chain targets.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
get_filename_component(_umicom_merkle_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
if(NOT TARGET Umicom::base)
    message(FATAL_ERROR "Merkle member proof requires existing Umicom::base")
endif()
if(NOT TARGET Umicom::record_chain)
    include("${CMAKE_CURRENT_LIST_DIR}/UmicomRecordChain.cmake")
endif()
if(NOT TARGET umicom_record_chain_merkle)
    add_library(umicom_record_chain_merkle STATIC
        "${_umicom_merkle_root}/src/record_chain/merkle.c")
    add_library(Umicom::record_chain_merkle ALIAS umicom_record_chain_merkle)
    target_include_directories(umicom_record_chain_merkle PUBLIC
        "${_umicom_merkle_root}/include")
    target_link_libraries(umicom_record_chain_merkle PUBLIC
        Umicom::record_chain Umicom::base)
    target_compile_features(umicom_record_chain_merkle PUBLIC c_std_23)
    if(MSVC)
        target_compile_options(umicom_record_chain_merkle PRIVATE /W4)
    else()
        target_compile_options(umicom_record_chain_merkle PRIVATE
            -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
    endif()
endif()
if(BUILD_TESTING AND NOT TARGET umicom-record-chain-merkle-test)
    add_executable(umicom-record-chain-merkle-test
        "${_umicom_merkle_root}/tests/record_chain/test_merkle.c")
    target_link_libraries(umicom-record-chain-merkle-test PRIVATE
        Umicom::record_chain_merkle)
    target_compile_features(umicom-record-chain-merkle-test PRIVATE c_std_23)
    add_test(NAME framework.record_chain.merkle_membership
             COMMAND umicom-record-chain-merkle-test)
    set_tests_properties(framework.record_chain.merkle_membership PROPERTIES
        LABELS "framework;record_chain;merkle;integrity" TIMEOUT 30)
endif()
