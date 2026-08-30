#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomPublicContractFingerprint.cmake
#
# PURPOSE:
#   Make every Framework consumer compile against one content fingerprint of
#   the public C headers. This protects incremental builds when copied or
#   restored files keep an old timestamp even though their contents changed.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)

function(umicom_compute_public_contract_fingerprint out_value header_root)
    if(NOT IS_DIRECTORY "${header_root}")
        message(FATAL_ERROR
            "Public contract header directory was not found: ${header_root}")
    endif()

    # CONFIGURE_DEPENDS asks CMake to configure again when a public header is
    # added or removed. Script-mode tests cannot use that flag, so they use the
    # same content calculation without build-system dependency tracking.
    if(DEFINED CMAKE_SCRIPT_MODE_FILE)
        file(GLOB_RECURSE _umicom_contract_headers
            LIST_DIRECTORIES FALSE
            "${header_root}/umicom/*.h")
    else()
        file(GLOB_RECURSE _umicom_contract_headers
            CONFIGURE_DEPENDS
            LIST_DIRECTORIES FALSE
            "${header_root}/umicom/*.h")
    endif()

    if(NOT _umicom_contract_headers)
        message(FATAL_ERROR
            "No public Umicom headers were found below: ${header_root}")
    endif()

    list(SORT _umicom_contract_headers)
    set(_umicom_contract_manifest "")

    foreach(_umicom_contract_header IN LISTS _umicom_contract_headers)
        file(RELATIVE_PATH _umicom_contract_relative
            "${header_root}"
            "${_umicom_contract_header}")
        file(SHA256 "${_umicom_contract_header}" _umicom_contract_hash)
        string(APPEND _umicom_contract_manifest
            "${_umicom_contract_relative}:${_umicom_contract_hash}\n")
    endforeach()

    string(SHA256 _umicom_contract_fingerprint
        "${_umicom_contract_manifest}")
    set(${out_value} "${_umicom_contract_fingerprint}" PARENT_SCOPE)
endfunction()

function(umicom_attach_public_contract_fingerprint target header_root)
    if(NOT TARGET "${target}")
        message(FATAL_ERROR
            "Cannot attach the public contract fingerprint to missing target: ${target}")
    endif()

    umicom_compute_public_contract_fingerprint(
        _umicom_public_contract_fingerprint
        "${header_root}")
    string(SUBSTRING
        "${_umicom_public_contract_fingerprint}"
        0 16
        _umicom_public_contract_fingerprint_short)

    # The definition is deliberately PUBLIC. A content change therefore
    # changes the compile command of Framework libraries, tests and client
    # applications even when a header's filesystem timestamp moves backwards.
    target_compile_definitions("${target}" PUBLIC
        "UMI_PUBLIC_CONTRACT_FINGERPRINT=0x${_umicom_public_contract_fingerprint_short}ULL")

    set_property(TARGET "${target}" PROPERTY
        UMICOM_PUBLIC_CONTRACT_FINGERPRINT
        "${_umicom_public_contract_fingerprint}")

    message(STATUS
        "Umicom public contract fingerprint: ${_umicom_public_contract_fingerprint_short}")
endfunction()

