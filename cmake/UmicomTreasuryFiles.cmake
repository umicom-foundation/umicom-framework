# Umicom Framework
# File: cmake/UmicomTreasuryFiles.cmake
# PURPOSE: Export optional treasury filesystem services without coupling portable finance to storage.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
include_guard(GLOBAL)
include("${CMAKE_CURRENT_LIST_DIR}/UmicomSdkComponent.cmake")
function(umicom_attach_treasury_files)
    if(TARGET umicom_treasury_files OR NOT TARGET Umicom::finance OR NOT TARGET Umicom::platform)
        return()
    endif()
    add_library(umicom_treasury_files STATIC
        "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../src/open_tms/fx_forward_file.c"
        "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../src/open_tms/accounting_file.c")
    add_library(Umicom::treasury_files ALIAS umicom_treasury_files)
    set_target_properties(umicom_treasury_files PROPERTIES EXPORT_NAME treasury_files
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    target_include_directories(umicom_treasury_files PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
    target_link_libraries(umicom_treasury_files PUBLIC Umicom::finance Umicom::platform)
    umicom_apply_warnings(umicom_treasury_files)
    umicom_apply_sanitizers(umicom_treasury_files)
    if(TARGET umicom_framework)
        target_link_libraries(umicom_framework INTERFACE Umicom::treasury_files)
    endif()
    install(TARGETS umicom_treasury_files EXPORT UmicomFrameworkTargets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Framework)
    umicom_register_sdk_libraries(umicom_treasury_files)

    if(COMMAND umicom_release_inventory_own_headers)
        # These storage entry points belong to the optional adapter, not finance.
        umicom_release_inventory_own_headers(umicom_treasury_files
            include/umicom/open_tms/fx_forward_file.h include/umicom/open_tms/accounting_file.h)
    endif()
    install(FILES
        "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../examples/accounting_files/main.c"
        "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../examples/accounting_files/CMakeLists.txt"
        DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/examples/accounting_files COMPONENT Learning)
    if(BUILD_TESTING)
        add_executable(umi-accounting-files-consumer
            "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../examples/accounting_files/main.c")
        target_link_libraries(umi-accounting-files-consumer PRIVATE Umicom::treasury_files)
        set_target_properties(umi-accounting-files-consumer PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        umicom_apply_warnings(umi-accounting-files-consumer)
        umicom_apply_sanitizers(umi-accounting-files-consumer)
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(umi-accounting-files-consumer)
        endif()
    endif()

    install(FILES "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../include/umicom/open_tms/fx_forward_file.h"
        "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../include/umicom/open_tms/accounting_file.h"
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/umicom/open_tms COMPONENT Framework)
    install(FILES
        "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../examples/treasury_forward_files/main.c"
        "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../examples/treasury_forward_files/CMakeLists.txt"
        DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom/examples/treasury_forward_files COMPONENT Learning)
    if(BUILD_TESTING)
        add_executable(umi-forward-file-test
            "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/open_tms/test_fx_forward_file.c")
        target_link_libraries(umi-forward-file-test PRIVATE Umicom::treasury_files)
        set_target_properties(umi-forward-file-test PROPERTIES
            C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        umicom_apply_warnings(umi-forward-file-test)
        umicom_apply_sanitizers(umi-forward-file-test)
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(umi-forward-file-test)
        endif()
        foreach(case roundtrip empty full no-receipt existing invalid-book null-book
            save-relative save-null save-directory save-missing-parent load-null-output
            missing load-relative load-null directory malformed zero oversize checksum trailing)
            set(test framework.open_tms.forward.file.${case})
            add_test(NAME ${test} COMMAND umi-forward-file-test ${case})
            set_tests_properties(${test} PROPERTIES TIMEOUT 30
                LABELS "framework;finance;treasury;files;persistence;regression")
            if(COMMAND umicom_test_impact_owners)
                umicom_test_impact_owners(${test} TARGETS umi-forward-file-test)
            endif()
        endforeach()

        add_executable(umi-accounting-file-test
            "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/open_tms/test_accounting_file.c")
        target_link_libraries(umi-accounting-file-test PRIVATE Umicom::treasury_files)
        set_target_properties(umi-accounting-file-test PROPERTIES
            C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        umicom_apply_warnings(umi-accounting-file-test)
        umicom_apply_sanitizers(umi-accounting-file-test)
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(umi-accounting-file-test)
        endif()
        if(COMMAND umicom_target_qualification_inputs)
            umicom_target_qualification_inputs(umi-accounting-file-test FILES
                "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/build_log/fixture.h")
        endif()
        foreach(case roundtrip empty full approved posted draft-unbalanced no-receipt
            existing invalid-book invalid-approved null-book save-relative save-null save-directory
            save-missing-parent load-null-output missing load-relative load-null directory malformed zero
            oversize checksum trailing)
            set(test framework.open_tms.accounting.file.${case})
            add_test(NAME ${test} COMMAND umi-accounting-file-test ${case})
            set_tests_properties(${test} PROPERTIES TIMEOUT 30
                LABELS "framework;finance;treasury;accounting;files;persistence;regression")
            if(COMMAND umicom_test_impact_owners)
                umicom_test_impact_owners(${test} TARGETS umi-accounting-file-test)
            endif()
        endforeach()
    endif()
endfunction()
umicom_attach_treasury_files()
if(NOT TARGET umicom_treasury_files)
    cmake_language(DEFER CALL umicom_attach_treasury_files)
endif()
