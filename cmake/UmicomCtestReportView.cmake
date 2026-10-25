# Umicom Framework
# File: cmake/UmicomCtestReportView.cmake
# PURPOSE: Register shared saved-report browsing and focused native qualification.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
include_guard(GLOBAL)
target_sources(umicom_testing PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/testing/ctest_report_view.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/testing/ctest_report_view_text.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/testing/ctest_report_file.c")
if(COMMAND umicom_target_qualification_inputs)
    umicom_target_qualification_inputs(umicom_testing FILES
        "${CMAKE_CURRENT_LIST_DIR}/../src/testing/ctest_report_view_private.h")
endif()
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/testing/ctest_report_view.h"
    "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/testing/ctest_report_file.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/testing" COMPONENT Framework)
if(COMMAND umicom_release_inventory_own_headers)
    umicom_release_inventory_own_headers(umicom_testing
        include/umicom/testing/ctest_report_view.h include/umicom/testing/ctest_report_file.h)
endif()
if(TARGET umicom_ui_gtk4)
    target_sources(umicom_ui_gtk4 PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../adapters/gtk4/ctest_report.c")
    target_link_libraries(umicom_ui_gtk4 PRIVATE Umicom::testing)
    install(FILES "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/ui/gtk4/ctest_report.h"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/ui/gtk4" COMPONENT Framework)
    if(COMMAND umicom_release_inventory_own_headers)
        umicom_release_inventory_own_headers(umicom_ui_gtk4 include/umicom/ui/gtk4/ctest_report.h)
    endif()
endif()
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/guides/saved-test-report-browser.html"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom-framework/docs" COMPONENT Framework)
if(NOT BUILD_TESTING)
    return()
endif()
function(umicom_saved_report_group group owner)
    set(target "umi-saved-report-${group}-test")
    add_executable(${target} "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/ctest_report_view/test_${group}.c")
    target_link_libraries(${target} PRIVATE ${owner})
    set_target_properties(${target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(${target})
    umicom_apply_sanitizers(${target})
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(${target})
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(${target} FILES
            "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/ctest_report_view/fixture.h"
            "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/build_log/fixture.h")
    endif()
    foreach(case IN LISTS ARGN)
        set(test "framework.ctest.saved_view.${group}.${case}")
        set(work "${CMAKE_CURRENT_BINARY_DIR}/test-work/saved-report/${group}/${case}")
        file(MAKE_DIRECTORY "${work}")
        add_test(NAME ${test} COMMAND ${target} ${case})
        set_tests_properties(${test} PROPERTIES TIMEOUT 60 WORKING_DIRECTORY "${work}"
            LABELS "framework;testing;saved-report;regression")
        if(group STREQUAL "gtk4")
            set_tests_properties(${test} PROPERTIES SKIP_RETURN_CODE 77 ENVIRONMENT "G_DEBUG=fatal-warnings")
        endif()
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(${test} TARGETS ${target})
        endif()
    endforeach()
endfunction()
umicom_saved_report_group(filter Umicom::testing default all passed failed timeout skipped disabled not-run none name name-order duration-order outcome-order copied-filter invalid-mask invalid-order invalid-utf8 unterminated null empty large tie-order)
umicom_saved_report_group(format Umicom::testing first middle last empty short query offset bounds unicode controls unchanged-source all-counts)
umicom_saved_report_group(file Umicom::testing valid empty malformed missing relative null directory ownership unicode)
if(TARGET umicom_ui_gtk4)
    umicom_saved_report_group(gtk4 Umicom::ui_gtk4 normal paging filter draft-filter outcomes missing invalid-filter hidden retained cancel-load hide-show close-load close-render captured-path clear)
endif()
