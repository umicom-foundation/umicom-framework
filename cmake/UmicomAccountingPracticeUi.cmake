# Umicom Framework
# File: cmake/UmicomAccountingPracticeUi.cmake
# PURPOSE: Compose the shared accounting batch surface after finance and GTK targets exist.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
include_guard(GLOBAL)
function(umicom_attach_accounting_practice_ui)
# The earlier finance-only boundary remains below for architectural review.
    if(FALSE)
    if(NOT TARGET umicom_ui_gtk4 OR NOT TARGET Umicom::finance)
        return()
    endif()
    endif()
    if(NOT TARGET umicom_ui_gtk4 OR NOT TARGET Umicom::treasury_files)
        return()
    endif()
    get_target_property(attached umicom_ui_gtk4 UMICOM_ACCOUNTING_PRACTICE_ATTACHED)
    if(attached)
        return()
    endif()
    include("${CMAKE_CURRENT_FUNCTION_LIST_DIR}/UmicomTextExport.cmake")
    target_sources(umicom_ui_gtk4 PRIVATE
        "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../adapters/gtk4/accounting_practice_gtk4.c"
        "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../adapters/gtk4/accounting_report_gtk4.c")
    # Keep portable finance independent of native storage. The optional file
    # component owns its finance and platform dependency closure.
    if(FALSE)
    target_link_libraries(umicom_ui_gtk4 PUBLIC Umicom::finance)
    endif()
    target_link_libraries(umicom_ui_gtk4 PUBLIC Umicom::treasury_files)
    target_compile_definitions(umicom_ui_gtk4 PUBLIC UMICOM_HAS_ACCOUNTING_PRACTICE=1)
    set_property(TARGET umicom_ui_gtk4 PROPERTY UMICOM_ACCOUNTING_PRACTICE_ATTACHED TRUE)
    if(COMMAND umicom_release_inventory_own_headers)
        umicom_release_inventory_own_headers(umicom_ui_gtk4
            include/umicom/ui/gtk4/accounting_practice.h include/umicom/ui/gtk4/accounting_report.h)
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom_ui_gtk4 FILES
            "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../adapters/gtk4/window_removal_private.h"
            "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../adapters/gtk4/accounting_practice_files.inc")
    endif()
    if(BUILD_TESTING)
        add_executable(umi-accounting-native-test
            "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/open_tms/test_accounting_native.c")
        target_link_libraries(umi-accounting-native-test PRIVATE Umicom::ui_gtk4)
        set_target_properties(umi-accounting-native-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        umicom_apply_warnings(umi-accounting-native-test)
        umicom_apply_sanitizers(umi-accounting-native-test)
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(umi-accounting-native-test)
        endif()
        foreach(case capture-pair approve post unbalanced cross-currency unacknowledged
            changed-form changed-action edit-preview close-preview parent ack-edit ack-click
            capture-nested posted-capture edit-approved repeat empty invalid-api invalid-action
            nan infinity hex comma whitespace currency empty-reference both-sides zero-sides)
            set(test framework.open_tms.accounting.native.${case})
            add_test(NAME ${test} COMMAND umi-accounting-native-test ${case})
            set_tests_properties(${test} PROPERTIES TIMEOUT 30 SKIP_RETURN_CODE 77
                ENVIRONMENT "G_DEBUG=fatal-warnings"
                LABELS "framework;finance;treasury;accounting;review;gtk4;regression")
            if(COMMAND umicom_test_impact_owners)
                umicom_test_impact_owners(${test} TARGETS umi-accounting-native-test)
            endif()
        endforeach()
    endif()

    if(BUILD_TESTING)
        add_executable(umi-accounting-files-native-test
            "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/open_tms/test_accounting_files_native.c")
        target_link_libraries(umi-accounting-files-native-test PRIVATE Umicom::ui_gtk4)
        set_target_properties(umi-accounting-files-native-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        umicom_apply_warnings(umi-accounting-files-native-test)
        umicom_apply_sanitizers(umi-accounting-files-native-test)
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(umi-accounting-files-native-test)
        endif()
        if(COMMAND umicom_target_qualification_inputs)
            umicom_target_qualification_inputs(umi-accounting-files-native-test FILES
                "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/build_log/fixture.h")
        endif()
        foreach(case save existing relative missing malformed invalid-utf8 review unacknowledged
            apply replay dirty-load edited-review revokes-action edit-preview close-preview ack-edit ack-click
            start-close start-edit close-pending save-snapshot save-close invalid-api)
            set(test framework.open_tms.accounting.files-native.${case})
            add_test(NAME ${test} COMMAND umi-accounting-files-native-test ${case})
            set_tests_properties(${test} PROPERTIES TIMEOUT 40 SKIP_RETURN_CODE 77
                ENVIRONMENT "G_DEBUG=fatal-warnings"
                LABELS "framework;finance;treasury;accounting;files;gtk4;regression")
            if(COMMAND umicom_test_impact_owners)
                umicom_test_impact_owners(${test} TARGETS umi-accounting-files-native-test)
            endif()
        endforeach()
    endif()

    if(BUILD_TESTING)
        add_executable(umi-accounting-report-native-test
            "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/open_tms/test_accounting_report_native.c")
        target_link_libraries(umi-accounting-report-native-test PRIVATE Umicom::ui_gtk4)
        set_target_properties(umi-accounting-report-native-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        umicom_apply_warnings(umi-accounting-report-native-test)
        umicom_apply_sanitizers(umi-accounting-report-native-test)
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(umi-accounting-report-native-test)
        endif()
        if(COMMAND umicom_target_qualification_inputs)
            umicom_target_qualification_inputs(umi-accounting-report-native-test FILES
                "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/build_log/fixture.h"
                "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/support/gtk4_expanders.h")
        endif()
        foreach(case empty balanced currencies quoted-text immutable released-source invalid-utf8 invalid-approved null-book null-output invalid-parent save existing retired parent-close accepted-close launcher)
            set(test framework.open_tms.accounting.report-native.${case})
            add_test(NAME ${test} COMMAND umi-accounting-report-native-test ${case})
            set_tests_properties(${test} PROPERTIES TIMEOUT 30 SKIP_RETURN_CODE 77
                ENVIRONMENT "G_DEBUG=fatal-warnings"
                LABELS "framework;finance;treasury;accounting;report;gtk4;regression")
            if(COMMAND umicom_test_impact_owners)
                umicom_test_impact_owners(${test} TARGETS umi-accounting-report-native-test)
            endif()
        endforeach()
    endif()
endfunction()
umicom_attach_accounting_practice_ui()
# Retry attachment when optional native file dependencies become available.
# The former finance-only retry remains disabled for review.
if(FALSE)
if(NOT TARGET umicom_ui_gtk4 OR NOT TARGET Umicom::finance)
    cmake_language(DEFER CALL umicom_attach_accounting_practice_ui)
endif()
endif()
if(NOT TARGET umicom_ui_gtk4 OR NOT TARGET Umicom::treasury_files)
    cmake_language(DEFER CALL umicom_attach_accounting_practice_ui)
endif()
install(FILES "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/ui/gtk4/accounting_practice.h"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom/ui/gtk4 COMPONENT Framework)
# Preserve the unconditional draft registration for review. Headless SDKs
# have no GTK target; ownership belongs inside the checked attachment below.
if(FALSE)
if(COMMAND umicom_release_inventory_own_headers)
    umicom_release_inventory_own_headers(umicom_ui_gtk4 include/umicom/ui/gtk4/accounting_practice.h)
endif()
endif()

install(FILES "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/ui/gtk4/accounting_report.h"
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom/ui/gtk4 COMPONENT Framework)
