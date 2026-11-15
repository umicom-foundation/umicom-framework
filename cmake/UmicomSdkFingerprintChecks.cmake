# Umicom Framework
# File: cmake/UmicomSdkFingerprintChecks.cmake
#
# PURPOSE:
# Register bounded SDK content observation and native CLI regressions.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
include_guard(GLOBAL)
if(NOT BUILD_TESTING)
    return()
endif()
add_executable(umicom-sdk-fingerprint-tests
    "${CMAKE_CURRENT_LIST_DIR}/../tests/sdk_installation/test_artifact_fingerprint.c")
target_link_libraries(umicom-sdk-fingerprint-tests PRIVATE Umicom::sdk)
umicom_apply_warnings(umicom-sdk-fingerprint-tests)
umicom_apply_sanitizers(umicom-sdk-fingerprint-tests)
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-sdk-fingerprint-tests)
endif()
foreach(_case vector unicode relocated same-size content-change unlisted empty zero-budget-empty multichunk receipt-crlf record-order configuration renamed-artifact shared-import repeat-path interfaces-only malformed traversal duplicate-targets missing reset-on-failure leaf-directory leaf-symlink absent receipt-directory receipt-symlink limit-file limit-total exact-budget zero-budget cancelled arguments)
    add_test(NAME "framework.sdk.fingerprint.value.${_case}"
        COMMAND umicom-sdk-fingerprint-tests "${_case}")
    set_tests_properties("framework.sdk.fingerprint.value.${_case}" PROPERTIES
        TIMEOUT 30 LABELS "framework;sdk;filesystem;regression" SKIP_RETURN_CODE 77)
endforeach()
if(TARGET umicom)
    foreach(_case complete match mismatch uppercase invalid-digest unknown-option missing malformed unicode relative help extra-argument empty-prefix directory)
        add_test(NAME "framework.sdk.fingerprint.cli.${_case}"
            COMMAND "${CMAKE_COMMAND}" "-DCLI_PROGRAM=$<TARGET_FILE:umicom>"
                "-DTEST_BINARY=${CMAKE_CURRENT_BINARY_DIR}/sdk-fingerprint-cli"
                "-DCASE=${_case}"
                -P "${CMAKE_CURRENT_LIST_DIR}/../tests/sdk_installation/check_fingerprint.cmake")
        set_tests_properties("framework.sdk.fingerprint.cli.${_case}" PROPERTIES
            TIMEOUT 30 LABELS "framework;sdk;cli;regression")
    endforeach()
endif()
