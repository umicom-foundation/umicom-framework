# Umicom Framework
# File: cmake/UmicomDebugConsoleReport.cmake
# PURPOSE: Register console capture, native paging and focused regression coverage.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
include_guard(GLOBAL)
target_sources(umicom_debug PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug/console_report.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug/console_report_text.c")
target_link_libraries(umicom_debug PRIVATE Umicom::document)
if(COMMAND umicom_target_qualification_inputs)
    umicom_target_qualification_inputs(umicom_debug FILES
        "${CMAKE_CURRENT_LIST_DIR}/../src/debug/console_report_private.h")
endif()
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/debug/console_report.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/debug" COMPONENT Framework)
if(COMMAND umicom_release_inventory_own_headers)
    umicom_release_inventory_own_headers(umicom_debug include/umicom/debug/console_report.h)
endif()
if(TARGET umicom_ui_gtk4)
    target_sources(umicom_ui_gtk4 PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../adapters/gtk4/debug_console.c")
    install(FILES "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/ui/gtk4/debug_console.h"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/ui/gtk4" COMPONENT Framework)
    if(COMMAND umicom_release_inventory_own_headers)
        umicom_release_inventory_own_headers(umicom_ui_gtk4 include/umicom/ui/gtk4/debug_console.h)
    endif()
endif()
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/guides/debugger-output-pages.html"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom-framework/docs" COMPONENT Framework)
if(NOT BUILD_TESTING)
    return()
endif()
function(umicom_console_report_group group owner)
    set(target "umi-console-report-${group}-test")
    add_executable(${target} "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/debug_console_report/test_${group}.c")
    target_link_libraries(${target} PRIVATE ${owner})
    set_target_properties(${target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(${target})
    umicom_apply_sanitizers(${target})
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(${target})
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(${target} FILES
            "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/debug_console_report/fixture.h")
    endif()
    foreach(case IN LISTS ARGN)
        set(test "framework.debug.console_report.${group}.${case}")
        add_test(NAME ${test} COMMAND ${target} ${case})
        set_tests_properties(${test} PROPERTIES TIMEOUT 45 LABELS "framework;debugger;console;regression")
        if(group STREQUAL "gtk4")
            set_tests_properties(${test} PROPERTIES SKIP_RETURN_CODE 77 ENVIRONMENT "G_DEBUG=fatal-warnings")
        endif()
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(${test} TARGETS ${target})
        endif()
    endforeach()
endfunction()
umicom_console_report_group(capture Umicom::debug ownership invalid-query invalid-utf8 null filtered limit unmatched maximum oldest newest severity ties)
umicom_console_report_group(format Umicom::debug first middle last empty offset bounds short controls unicode maximum truncated entry-bounds)
if(TARGET umicom_ui_gtk4)
    umicom_console_report_group(gtk4 Umicom::ui_gtk4 normal paging draft-filter filter failure invalid-filter hidden retained close-query close-render clear hidden-copy)
endif()
