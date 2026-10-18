# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Report import reuses the canonical complete JSON reader. Keep it separate from
# the socket owner so minimal TWS transports do not acquire IDE dependencies.
# This module also supports independent composition; own its SDK registration dependency.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomSdkComponent.cmake")
include_guard(GLOBAL)
if(NOT TARGET Umicom::developer)
    return()
endif()
add_library(umicom_ibkr_rule_report STATIC
    "${CMAKE_CURRENT_LIST_DIR}/../src/ibkr_connection/rule_report.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ibkr_connection/rule_report_decode.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ibkr_connection/rule_report_text.c")
add_library(Umicom::ibkr_rule_report ALIAS umicom_ibkr_rule_report)
set_target_properties(umicom_ibkr_rule_report PROPERTIES EXPORT_NAME ibkr_rule_report)
target_include_directories(umicom_ibkr_rule_report PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(umicom_ibkr_rule_report PRIVATE Umicom::developer)
umicom_ibkr_target(umicom_ibkr_rule_report)
install(TARGETS umicom_ibkr_rule_report EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Framework)
# Include this exported library surface in the opt-in SDK without changing full installs.
umicom_register_sdk_libraries(
    umicom_ibkr_rule_report
)
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/broker_connectivity/rule_report.h"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom/broker_connectivity COMPONENT Framework)
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/review-contract-rule-reports.html"
    DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/learning COMPONENT Learning)
if(BUILD_TESTING)
    add_executable(umicom-ibkr-rule-report-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ibkr_connection/test_rule_report.c")
    target_link_libraries(umicom-ibkr-rule-report-test PRIVATE Umicom::ibkr_rule_report)
    umicom_ibkr_target(umicom-ibkr-rule-report-test)
    foreach(case values missing empty inconclusive spoof flags tokens sell duplicate duplicate-route
        duplicate-escaped invalid-json unicode control route-space stock-only bad-side bad-id
        fractional-id large-id timestamp leap-date bad-date bad-number bool-number raw-numbers
        missing-identity wrong-list long-entry too-many-entries too-many-routes depth byte-limit
        ownership entry-bounds output-unchanged contract-mismatch side-mismatch route-mismatch
        route-case match no-account-output)
        add_test(NAME framework.ibkr_rule_report.${case} COMMAND umicom-ibkr-rule-report-test ${case})
        set_tests_properties(framework.ibkr_rule_report.${case} PROPERTIES
            TIMEOUT 20 LABELS "framework;broker;metadata;full-quantity;regression")
    endforeach()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-ibkr-rule-report-test)
    endif()
endif()
