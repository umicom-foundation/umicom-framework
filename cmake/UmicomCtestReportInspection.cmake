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
