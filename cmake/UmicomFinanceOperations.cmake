#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomFinanceOperations.cmake
#
# PURPOSE:
#   Build shared accounting, exchange and settlement operations once, then attach
#   small application compositions without rebuilding canonical dependencies.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
include(GNUInstallDirs)
include(CMakeParseArguments)
set(_umicom_finance_operations_root "${CMAKE_CURRENT_LIST_DIR}/..")
foreach(dependency IN ITEMS Umicom::finance Umicom::trading Umicom::data)
    if(NOT TARGET ${dependency})
        message(FATAL_ERROR "Umicom financial operations require ${dependency}")
    endif()
endforeach()

# This helper configures new targets only. Warning and instrumentation policy
# comes from the existing Framework build whenever it supplies those helpers.
function(umicom_finance_operations_configure_target target)
    set_target_properties(${target} PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(${target})
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(${target})
    endif()
endfunction()

add_library(umicom_finance_operations STATIC
    "${_umicom_finance_operations_root}/src/finance_operations/runtime.c"
    "${_umicom_finance_operations_root}/src/finance_operations/validation.c"
    "${_umicom_finance_operations_root}/src/finance_operations/availability.c"
    "${_umicom_finance_operations_root}/src/finance_operations/accounting.c"
    "${_umicom_finance_operations_root}/src/finance_operations/exchange.c"
    "${_umicom_finance_operations_root}/src/finance_operations/settlement.c"
    "${_umicom_finance_operations_root}/src/finance_operations/reporting.c"
    "${_umicom_finance_operations_root}/src/finance_operations/codec.c"
    "${_umicom_finance_operations_root}/src/finance_operations/repository.c"
    "${_umicom_finance_operations_root}/src/finance_operations/input.c")
add_library(Umicom::finance_operations ALIAS umicom_finance_operations)
set_target_properties(umicom_finance_operations PROPERTIES EXPORT_NAME finance_operations)
target_include_directories(umicom_finance_operations PUBLIC
    $<BUILD_INTERFACE:${_umicom_finance_operations_root}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(umicom_finance_operations PUBLIC
    Umicom::finance Umicom::trading Umicom::data)
umicom_finance_operations_configure_target(umicom_finance_operations)
install(TARGETS umicom_finance_operations EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Framework)
install(DIRECTORY "${_umicom_finance_operations_root}/include/umicom/finance_operations"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom" COMPONENT Framework)
install(FILES "${_umicom_finance_operations_root}/include/umicom/developer_operations/projection.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/developer_operations" COMPONENT Framework)

# GTK is an optional presentation adapter. The core and examples remain usable
# without any graphical dependency and never open a window as part of a test.
if(TARGET Umicom::ui_gtk4)
    add_library(umicom_finance_operations_gtk4 STATIC
        "${_umicom_finance_operations_root}/adapters/gtk4/finance_operations_panel_gtk4.c"
        "${_umicom_finance_operations_root}/adapters/gtk4/finance_operations_report_gtk4.c"
        "${_umicom_finance_operations_root}/adapters/gtk4/finance_operations_application_gtk4.c")
    add_library(Umicom::finance_operations_gtk4 ALIAS umicom_finance_operations_gtk4)
    set_target_properties(umicom_finance_operations_gtk4 PROPERTIES EXPORT_NAME finance_operations_gtk4)
    target_link_libraries(umicom_finance_operations_gtk4 PUBLIC
        Umicom::finance_operations Umicom::ui_gtk4)
    umicom_finance_operations_configure_target(umicom_finance_operations_gtk4)
    install(TARGETS umicom_finance_operations_gtk4 EXPORT UmicomFrameworkTargets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Framework)
    install(FILES "${_umicom_finance_operations_root}/include/umicom/ui/gtk4/finance_operations.h"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/ui/gtk4" COMPONENT Framework)
    install(FILES "${_umicom_finance_operations_root}/include/umicom/application/suite_layout/gtk4_product_content_extension.h"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/application/suite_layout" COMPONENT Framework)
endif()

if(BUILD_TESTING)
    add_executable(umicom-finance-operations-test
        "${_umicom_finance_operations_root}/tests/finance_operations/test_operations.c")
    target_link_libraries(umicom-finance-operations-test PRIVATE Umicom::finance_operations)
    umicom_finance_operations_configure_target(umicom-finance-operations-test)
    include("${_umicom_finance_operations_root}/tests/finance_operations/cases.cmake")
    foreach(case IN LISTS UMICOM_FINANCE_OPERATION_CASES)
        add_test(NAME "framework.finance_operations.${case}"
            COMMAND umicom-finance-operations-test "${case}")
        set_tests_properties("framework.finance_operations.${case}" PROPERTIES
            TIMEOUT 45 LABELS "framework;finance-operations;regression")
    endforeach()
    foreach(case IN LISTS UMICOM_FINANCE_OPERATION_SQLITE_CASES)
        add_test(NAME "framework.finance_operations.sqlite.${case}"
            COMMAND umicom-finance-operations-test "sqlite.${case}"
                "${CMAKE_CURRENT_BINARY_DIR}/finance-operations-${case}.sqlite")
        set_tests_properties("framework.finance_operations.sqlite.${case}" PROPERTIES
            TIMEOUT 45 SKIP_RETURN_CODE 77 LABELS "framework;finance-operations;sqlite;regression")
    endforeach()
    if(TARGET Umicom::developer)
        add_executable(umicom-developer-projection-coexistence-test
            "${_umicom_finance_operations_root}/tests/developer_operations/test_projection_coexistence.c"
            "${_umicom_finance_operations_root}/tests/developer_operations/projection_reverse_include.c"
            "${_umicom_finance_operations_root}/tests/developer_operations/projection_legacy_probe.c")
        target_link_libraries(umicom-developer-projection-coexistence-test PRIVATE Umicom::developer)
        umicom_finance_operations_configure_target(umicom-developer-projection-coexistence-test)
        add_test(NAME framework.developer_operations.projection_coexistence
            COMMAND umicom-developer-projection-coexistence-test)
        set_tests_properties(framework.developer_operations.projection_coexistence PROPERTIES
            TIMEOUT 30 LABELS "framework;developer-operations;regression")
    endif()
    if(TARGET Umicom::finance_operations_gtk4)
        add_executable(umicom-finance-operations-gtk-test
            "${_umicom_finance_operations_root}/tests/finance_operations/test_panel_gtk4.c")
        target_link_libraries(umicom-finance-operations-gtk-test PRIVATE Umicom::finance_operations_gtk4)
        umicom_finance_operations_configure_target(umicom-finance-operations-gtk-test)
        add_test(NAME framework.finance_operations.gtk_panel COMMAND umicom-finance-operations-gtk-test)
        set_tests_properties(framework.finance_operations.gtk_panel PROPERTIES
            TIMEOUT 40 SKIP_RETURN_CODE 77 LABELS "framework;finance-operations;gtk4;regression")
    endif()
endif()

# The function's own definition directory, not the first caller's directory,
# locates the example. This works when both child modules include this helper.
function(umicom_attach_finance_operations)
    cmake_parse_arguments(PARSE_ARGV 0 UMI "" "NAME;TARGET" "")
    if(UMI_UNPARSED_ARGUMENTS OR NOT UMI_NAME OR NOT UMI_TARGET)
        message(FATAL_ERROR "Financial operations require NAME and TARGET")
    endif()
    if(NOT TARGET "${UMI_TARGET}")
        message(FATAL_ERROR "Missing financial application module ${UMI_TARGET}")
    endif()
    target_link_libraries("${UMI_TARGET}" PUBLIC Umicom::finance_operations)
    set(example "umicom-${UMI_NAME}-operations-example")
    add_executable("${example}"
        "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../examples/finance_operations_main.c")
    target_link_libraries("${example}" PRIVATE Umicom::finance_operations)
    umicom_finance_operations_configure_target("${example}")
    install(TARGETS "${example}" RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
    if(BUILD_TESTING)
        add_test(NAME "${UMI_NAME}.operations.example" COMMAND "${example}")
        set_tests_properties("${UMI_NAME}.operations.example" PROPERTIES
            TIMEOUT 30 LABELS "${UMI_NAME};finance-operations;example")
    endif()
endfunction()

install(FILES "${_umicom_finance_operations_root}/docs/learning/ACCOUNTANT_EXCHANGE.html"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/docs/learning" COMPONENT Framework)
unset(_umicom_finance_operations_root)
