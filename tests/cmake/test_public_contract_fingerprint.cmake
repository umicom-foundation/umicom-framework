#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/cmake/test_public_contract_fingerprint.cmake
#
# PURPOSE:
#   Verify that a public header content change produces a different Framework
#   contract fingerprint even when the path and number of headers stay equal.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------
include("${CMAKE_CURRENT_LIST_DIR}/../../cmake/UmicomPublicContractFingerprint.cmake")

set(_umicom_fixture_root
    "${CMAKE_CURRENT_BINARY_DIR}/umicom-public-contract-fingerprint-fixture")
file(MAKE_DIRECTORY "${_umicom_fixture_root}/umicom/example")
file(WRITE "${_umicom_fixture_root}/umicom/example/contract.h"
    "#define UMI_EXAMPLE_MEMBER_COUNT 22U\n")

umicom_compute_public_contract_fingerprint(
    _umicom_before
    "${_umicom_fixture_root}")

file(WRITE "${_umicom_fixture_root}/umicom/example/contract.h"
    "#define UMI_EXAMPLE_MEMBER_COUNT 26U\n")

umicom_compute_public_contract_fingerprint(
    _umicom_after
    "${_umicom_fixture_root}")

if(_umicom_before STREQUAL _umicom_after)
    message(FATAL_ERROR
        "Public contract fingerprint did not change with header content.")
endif()

