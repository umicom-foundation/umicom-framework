# -----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomCtestReportInspection.cmake
# Purpose: Register shared saved-report inspection and its regression checks.
# Author: Sammy Hegab | Umicom Foundation | MIT
# -----------------------------------------------------------------------------
include_guard(GLOBAL)
if(TARGET umicom)
    target_sources(umicom PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../tools/umicom/src/command_test_report.c")
endif()
if(BUILD_TESTING AND TARGET umicom_testing)
    add_executable(umicom-ctest-report-collection-checks
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ctest_execution/test_report_collection.c")
    target_link_libraries(umicom-ctest-report-collection-checks PRIVATE Umicom::testing)
    umicom_apply_warnings(umicom-ctest-report-collection-checks)
    umicom_apply_sanitizers(umicom-ctest-report-collection-checks)
    include("${CMAKE_CURRENT_LIST_DIR}/../tests/ctest_execution/collection_checks.cmake")
    if(TARGET umicom)
        foreach(scenario help passed problems all empty incomplete console missing unknown-argument unicode)
            add_test(NAME "framework.ctest.report_cli.${scenario}"
                COMMAND "${CMAKE_COMMAND}"
                    "-DUMICOM_COMMAND=$<TARGET_FILE:umicom>"
                    "-DCASE=${scenario}"
                    "-DWORK=${CMAKE_CURRENT_BINARY_DIR}/test-work/report-cli/${scenario}"
                    -P "${CMAKE_CURRENT_LIST_DIR}/../tests/ctest_execution/check_report_cli.cmake")
            set_tests_properties("framework.ctest.report_cli.${scenario}" PROPERTIES TIMEOUT 30)
        endforeach()
    endif()
endif()
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/inspect-test-reports.html"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/docs/learning" COMPONENT Learning)

# Compare saved observations through the shared service; the CLI does not own
# outcome policy. Each case uses its own directory to support parallel CTest.
if(BUILD_TESTING AND TARGET umicom_testing)
    add_executable(umicom-ctest-report-comparison-checks
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ctest_execution/test_report_comparison.c")
    target_link_libraries(umicom-ctest-report-comparison-checks PRIVATE Umicom::testing)
    umicom_apply_warnings(umicom-ctest-report-comparison-checks)
    umicom_apply_sanitizers(umicom-ctest-report-comparison-checks)
    foreach(scenario from-passed from-failed from-timeout from-skipped from-disabled from-not-run
        inventory order-and-details names-and-ownership empty invalid large)
        add_test(NAME "framework.ctest.report_comparison.${scenario}"
            COMMAND umicom-ctest-report-comparison-checks "${scenario}")
        set_tests_properties("framework.ctest.report_comparison.${scenario}" PROPERTIES TIMEOUT 45)
    endforeach()
    if(TARGET umicom)
        foreach(scenario recovered unchanged-failure removed-failure added-pass regressed-skip
            disabled-skip empty unicode control-name literal-name incomplete-before incomplete-after missing-before missing-after)
            add_test(NAME "framework.ctest.report_comparison_cli.${scenario}"
                COMMAND "${CMAKE_COMMAND}"
                    "-DUMICOM_COMMAND=$<TARGET_FILE:umicom>"
                    "-DCASE=${scenario}"
                    "-DWORK=${CMAKE_CURRENT_BINARY_DIR}/test-work/report-comparison-cli/${scenario}"
                    -P "${CMAKE_CURRENT_LIST_DIR}/../tests/ctest_execution/check_report_comparison_cli.cmake")
            set_tests_properties("framework.ctest.report_comparison_cli.${scenario}" PROPERTIES TIMEOUT 30)
        endforeach()
    endif()
endif()

# Native saved-coverage review keeps publication and execution separate.
if(BUILD_TESTING AND TARGET umicom AND TARGET umicom_testing)
    foreach(scenario passed skipped stale missing-host missing-test configuration empty duplicate
        parent-path malformed missing-report two-hosts unicode relative-manifest
        not-run disabled message-controls paging)
        set(test "framework.ctest.coverage_cli.${scenario}")
        add_test(NAME ${test}
            COMMAND "${CMAKE_COMMAND}"
                "-DUMICOM_COMMAND=$<TARGET_FILE:umicom>"
                "-DCASE=${scenario}"
                "-DWORK=${CMAKE_CURRENT_BINARY_DIR}/test-work/coverage-cli/${scenario}"
                -P "${CMAKE_CURRENT_LIST_DIR}/../tests/ctest_execution/check_coverage_cli.cmake")
        set_tests_properties(${test} PROPERTIES TIMEOUT 30 LABELS "framework;testing;saved-report;coverage")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(${test} TARGETS umicom umicom_testing)
        endif()
    endforeach()
endif()
