# Umicom Framework
# File: cmake/UmicomSelfMemoryReports.cmake
#
# PURPOSE:
#   Build shared memory reports and register precision and output-boundary checks.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

include_guard(GLOBAL)
get_filename_component(_memory_report_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
target_sources(umicom_desktop_system PRIVATE "${_memory_report_root}/src/desktop_system/self_memory_report.c")
if(BUILD_TESTING)
    add_executable(umicom-self-memory-report-test "${_memory_report_root}/tests/desktop_system/test_self_memory_report.c")
    target_link_libraries(umicom-self-memory-report-test PRIVATE Umicom::desktop_system)
    set_target_properties(umicom-self-memory-report-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-self-memory-report-test)
    umicom_apply_sanitizers(umicom-self-memory-report-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-self-memory-report-test)
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-self-memory-report-test FILES
            "${_memory_report_root}/src/desktop_system/self_memory_report.c"
            "${_memory_report_root}/include/umicom/desktop_system/self_memory_report.h"
            "${_memory_report_root}/src/desktop_system/self_memory.c")
    endif()
    foreach(_case IN ITEMS valid maximum unknown small zero-capacity null-sample null-stage empty-stage long-stage quote-stage newline-stage utf8-stage null-buffer null-written invalid-sample exact-fit no-terminator report-valid report-capture-failure report-invalid-sample report-short-write report-flush-failure report-stream-error report-null-stream report-invalid-stage)
        add_test(NAME "framework.desktop_system.self_memory_report.${_case}" COMMAND umicom-self-memory-report-test "${_case}")
        set_tests_properties("framework.desktop_system.self_memory_report.${_case}" PROPERTIES TIMEOUT 20 LABELS "framework;desktop-system;memory;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners("framework.desktop_system.self_memory_report.${_case}" TARGETS umicom-self-memory-report-test)
        endif()
    endforeach()
endif()
unset(_memory_report_root)
