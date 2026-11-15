#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomCodeGuardReports.cmake
#
# PURPOSE:
# Register headless URI and CodeGuard report output regressions.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

if(BUILD_TESTING)
    add_executable(umicom-path-uri-tests "${CMAKE_CURRENT_LIST_DIR}/../tests/test_path_uri.c")
    target_link_libraries(umicom-path-uri-tests PRIVATE Umicom::platform)
    target_compile_features(umicom-path-uri-tests PRIVATE c_std_23)
    umicom_apply_warnings(umicom-path-uri-tests)
    umicom_apply_sanitizers(umicom-path-uri-tests)
    foreach(_case IN ITEMS relative reserved unicode colon absolute capacity alias invalid-utf8 empty relative-rejected too-long arguments root-context)
        add_test(NAME "framework.path_uri.${_case}" COMMAND umicom-path-uri-tests "${_case}")
        set_tests_properties("framework.path_uri.${_case}" PROPERTIES TIMEOUT 20 LABELS "platform;uri;ownership")
    endforeach()

    add_executable(umicom-codeguard-report-contract-tests
        "${CMAKE_CURRENT_LIST_DIR}/../tests/codeguard/test_report_contract.c")
    # The JSON parser is owned by developer; a header directory is not a target alias.
    # Retain the failed test wiring for review; it never enters the build graph.
    if(FALSE)
        target_link_libraries(umicom-codeguard-report-contract-tests PRIVATE Umicom::codeguard Umicom::language_runtime)
    else()
        target_link_libraries(umicom-codeguard-report-contract-tests PRIVATE Umicom::codeguard Umicom::developer)
    endif()
    target_compile_features(umicom-codeguard-report-contract-tests PRIVATE c_std_23)
    umicom_apply_warnings(umicom-codeguard-report-contract-tests)
    umicom_apply_sanitizers(umicom-codeguard-report-contract-tests)
    set(_report_root "${CMAKE_CURRENT_BINARY_DIR}/tests/codeguard-report-contract")
    file(MAKE_DIRECTORY "${_report_root}")
    foreach(_case IN ITEMS empty-text empty-json empty-sarif json-fields json-controls sarif-controls sarif-multiple sarif-unknown-line sarif-unknown-column sarif-no-location uri-relative uri-unicode uri-reserved text-fields maximum-fields render-budget render-zero-budget arguments unterminated-rule unterminated-cwe unterminated-path unterminated-message unterminated-remediation invalid-utf8-rule invalid-utf8-cwe invalid-utf8-path invalid-utf8-message invalid-utf8-remediation invalid-severity invalid-category invalid-confidence invalid-format-file null-result-file invalid-finding-file unicode-file stream-readonly existing-stream-error stream-equivalence)
        add_test(NAME "framework.codeguard_report.${_case}"
            COMMAND umicom-codeguard-report-contract-tests "${_case}" "${_report_root}")
        set_tests_properties("framework.codeguard_report.${_case}" PROPERTIES
            TIMEOUT 20 LABELS "codeguard;report;serialization")
    endforeach()

    # Linker wrapping targets the report's real output/allocation boundaries.
    # Other toolchains retain all ordinary tests; no fault cases are disguised
    # as successful no-ops when this linker facility is unavailable.
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang" AND NOT APPLE AND NOT MSVC)
        add_executable(umicom-codeguard-report-fault-tests
            "${CMAKE_CURRENT_LIST_DIR}/../tests/codeguard/test_report_contract.c")
        target_compile_definitions(umicom-codeguard-report-fault-tests PRIVATE UMICOM_REPORT_FAULTS=1)
        # The JSON parser is owned by developer; a header directory is not a target alias.
    # Retain the failed test wiring for review; it never enters the build graph.
    if(FALSE)
        target_link_libraries(umicom-codeguard-report-fault-tests PRIVATE Umicom::codeguard Umicom::language_runtime)
    else()
        target_link_libraries(umicom-codeguard-report-fault-tests PRIVATE Umicom::codeguard Umicom::developer)
    endif()
        target_compile_features(umicom-codeguard-report-fault-tests PRIVATE c_std_23)
        target_link_options(umicom-codeguard-report-fault-tests PRIVATE
            "-Wl,--wrap=fwrite" "-Wl,--wrap=fflush" "-Wl,--wrap=malloc"
            "-Wl,--wrap=umi_fs_write_bytes")
        umicom_apply_warnings(umicom-codeguard-report-fault-tests)
        umicom_apply_sanitizers(umicom-codeguard-report-fault-tests)
        foreach(_case IN ITEMS write-failure flush-failure allocation-failure file-write-failure)
            add_test(NAME "framework.codeguard_report.${_case}"
                COMMAND umicom-codeguard-report-fault-tests "${_case}" "${_report_root}")
            set_tests_properties("framework.codeguard_report.${_case}" PROPERTIES
                TIMEOUT 20 LABELS "codeguard;report;fault-injection")
        endforeach()
    endif()
endif()
