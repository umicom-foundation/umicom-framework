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
    # Exercise explicit windows on the same CLI entry point as the default receipt checks.
    foreach(_case complete match mismatch uppercase invalid-digest unknown-option missing malformed unicode relative help extra-argument empty-prefix directory
            budget-exact budget-file-only budget-total-only budget-file-small budget-total-small budget-empty
            budget-duplicate budget-duplicate-expect budget-missing budget-suffix budget-sign budget-whitespace budget-overflow budget-file-ceiling budget-total-ceiling)
        add_test(NAME "framework.sdk.fingerprint.cli.${_case}"
            COMMAND "${CMAKE_COMMAND}" "-DCLI_PROGRAM=$<TARGET_FILE:umicom>"
                "-DTEST_BINARY=${CMAKE_CURRENT_BINARY_DIR}/sdk-fingerprint-cli"
                "-DCASE=${_case}"
                -P "${CMAKE_CURRENT_LIST_DIR}/../tests/sdk_installation/check_fingerprint.cmake")
        set_tests_properties("framework.sdk.fingerprint.cli.${_case}" PROPERTIES
            TIMEOUT 30 LABELS "framework;sdk;cli;regression")
    endforeach()
endif()


# Controlled stream faults exercise the production SDK command and real SDK
# inspection services without closing stdout or depending on /dev/full.
add_executable(umicom-sdk-report-output-tests
    "${CMAKE_CURRENT_LIST_DIR}/../tests/sdk_installation/test_report_output.c")
target_link_libraries(umicom-sdk-report-output-tests PRIVATE Umicom::sdk Umicom::platform)
set_target_properties(umicom-sdk-report-output-tests PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES)
umicom_apply_warnings(umicom-sdk-report-output-tests)
umicom_apply_sanitizers(umicom-sdk-report-output-tests)
if(COMMAND umicom_target_qualification_inputs)
    umicom_target_qualification_inputs(umicom-sdk-report-output-tests FILES
        "${CMAKE_CURRENT_LIST_DIR}/../tools/umicom/src/command_sdk.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/support/report_stream.h")
endif()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-sdk-report-output-tests)
endif()
foreach(_route IN ITEMS inspect-complete inspect-incomplete artifacts-complete artifacts-missing
        artifacts-empty fingerprint-match fingerprint-mismatch help help-alias)
    foreach(_failure IN ITEMS success write-first write-last flush stream flush-and-stream)
        add_test(NAME framework.sdk.report_output.${_route}.${_failure}
            COMMAND umicom-sdk-report-output-tests ${_route} ${_failure})
        set_tests_properties(framework.sdk.report_output.${_route}.${_failure} PROPERTIES
            TIMEOUT 20 LABELS "framework;sdk;cli;output;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(framework.sdk.report_output.${_route}.${_failure}
                TARGETS umicom-sdk-report-output-tests)
        endif()
    endforeach()
endforeach()
