#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomWindowsReleaseSigning.cmake
#
# PURPOSE:
#   Explicit opt-in Authenticode signing of a REVIEWED release executable.
#   Debug/test binaries are never signed through this helper. It neither
#   purchases a certificate nor approves a quarantined file for execution.
#
# SECURITY:
#   Call this helper only after source review, dependency inspection, clean
#   release tests and antivirus-vendor disposition. The certificate's PRIVATE
#   KEY must remain in a protected Windows store/hardware signer, not Git.
#   Never disable antivirus or silently continue after signing failure.
#
# AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation | MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)

function(umicom_attach_windows_release_signing target)
    if(NOT WIN32)
        message(FATAL_ERROR "Authenticode signing requires a Windows build host")
    endif()
    if(NOT TARGET "${target}")
        message(FATAL_ERROR "Cannot sign unknown target: ${target}")
    endif()
    get_target_property(_umicom_target_type "${target}" TYPE)
    if(NOT _umicom_target_type STREQUAL "EXECUTABLE" AND
       NOT _umicom_target_type STREQUAL "SHARED_LIBRARY")
        message(FATAL_ERROR "Signing supports only executables and shared libraries")
    endif()
    # Fail closed on multi-configuration generators: this first integration
    # cannot prove the active binary is the reviewed Release configuration.
    if(CMAKE_CONFIGURATION_TYPES OR NOT CMAKE_BUILD_TYPE STREQUAL "Release")
        message(FATAL_ERROR "Use a single-configuration Release build to sign")
    endif()
    if(NOT DEFINED UMICOM_WINDOWS_SIGNING_APPROVED OR
       NOT UMICOM_WINDOWS_SIGNING_APPROVED)
        message(FATAL_ERROR "Explicitly approve this reviewed release before signing")
    endif()
    if(NOT DEFINED UMICOM_WINDOWS_SIGNER_THUMBPRINT)
        message(FATAL_ERROR "Provide the trusted certificate thumbprint")
    endif()
    string(LENGTH "${UMICOM_WINDOWS_SIGNER_THUMBPRINT}" _umicom_fingerprint_size)
    if(NOT _umicom_fingerprint_size EQUAL 40 OR
       NOT UMICOM_WINDOWS_SIGNER_THUMBPRINT MATCHES "^[0-9A-Fa-f]+$")
        message(FATAL_ERROR "Provide exactly 40 hexadecimal thumbprint characters")
    endif()
    if(NOT DEFINED UMICOM_WINDOWS_TIMESTAMP_URL OR
       NOT UMICOM_WINDOWS_TIMESTAMP_URL MATCHES "^https://[A-Za-z0-9._:/-]+$")
        message(FATAL_ERROR "Provide a reviewed RFC 3161 HTTPS timestamp URL")
    endif()
    if(DEFINED UMICOM_WINDOWS_SIGNTOOL AND NOT UMICOM_WINDOWS_SIGNTOOL STREQUAL "")
        set(_umicom_signtool "${UMICOM_WINDOWS_SIGNTOOL}")
        if(NOT EXISTS "${_umicom_signtool}")
            message(FATAL_ERROR "Configured SignTool executable does not exist")
        endif()
    else()
        find_program(_umicom_signtool NAMES signtool.exe REQUIRED)
    endif()
    add_custom_command(TARGET "${target}" POST_BUILD
        COMMAND "${_umicom_signtool}" sign
            /fd SHA256 /sha1 "${UMICOM_WINDOWS_SIGNER_THUMBPRINT}"
            /tr "${UMICOM_WINDOWS_TIMESTAMP_URL}" /td SHA256
            "$<TARGET_FILE:${target}>"
        COMMAND "${_umicom_signtool}" verify /pa /v
            "$<TARGET_FILE:${target}>"
        COMMENT "Sign and verify reviewed Umicom Windows release (opt-in)"
        VERBATIM)
endfunction()
