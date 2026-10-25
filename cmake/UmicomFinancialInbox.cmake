#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomFinancialInbox.cmake
# PURPOSE: Compose transaction-owned financial receipts above finance and Data Server.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
if(NOT TARGET umicom_finance OR NOT TARGET umicom_data)
    return()
endif()
include_guard(GLOBAL)
include("${CMAKE_CURRENT_LIST_DIR}/UmicomSdkComponent.cmake")
include(GNUInstallDirs)
get_filename_component(_ufmp_inbox_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
# Keep persistence above finance and Data Server to avoid a messaging dependency cycle.
add_library(umicom_financial_inbox STATIC
    "${_ufmp_inbox_root}/src/financial_messaging/inbox_codec.c"
    "${_ufmp_inbox_root}/src/financial_messaging/inbox_store.c")
add_library(Umicom::financial_inbox ALIAS umicom_financial_inbox)
set_target_properties(umicom_financial_inbox PROPERTIES EXPORT_NAME financial_inbox
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
target_link_libraries(umicom_financial_inbox PUBLIC Umicom::finance Umicom::data)
target_include_directories(umicom_financial_inbox PUBLIC
    "$<BUILD_INTERFACE:${_ufmp_inbox_root}/include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
umicom_apply_warnings(umicom_financial_inbox)
umicom_apply_sanitizers(umicom_financial_inbox)
install(TARGETS umicom_financial_inbox EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT Framework)
umicom_register_sdk_libraries(umicom_financial_inbox)
install(FILES "${_ufmp_inbox_root}/include/umicom/financial_messaging/inbox_store.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/financial_messaging" COMPONENT Framework)
if(COMMAND umicom_release_inventory_own_headers)
    umicom_release_inventory_own_headers(umicom_financial_inbox "include/umicom/financial_messaging/inbox_store.h")
endif()
if(COMMAND umicom_target_qualification_inputs)
    umicom_target_qualification_inputs(umicom_financial_inbox FILES
        "${_ufmp_inbox_root}/src/financial_messaging/inbox_internal.h")
endif()
if(BUILD_TESTING)
    set(_inbox_store_cases record retry new-delivery alias-retry payload-conflict recipient-conflict message-conflict sender-scope binary capacity expired transaction-owner corrupt-record missing-record corrupt-alias corrupt-operation missing-receipt receipt-syntax no-operation-key output-preserved maximum-fields)
    set(_inbox_codec_cases roundtrip binary-zero truncated extra-field version overflow leading-zero unknown-type bad-hex wrong-digest no-terminator)
    get_target_property(_inbox_data_definitions umicom_data INTERFACE_COMPILE_DEFINITIONS)
    set(_inbox_groups store codec)
    # Absent SQLite means these checks are unavailable, not passing or skipped.
    if("UMICOM_HAS_SQLITE=1" IN_LIST _inbox_data_definitions)
        list(APPEND _inbox_groups sqlite)
        set(_inbox_sqlite_cases reopen readonly two-connections)
    endif()
    foreach(_inbox_group IN LISTS _inbox_groups)
        set(_inbox_target "umicom-ufmp-inbox-${_inbox_group}-test")
        add_executable(${_inbox_target} "${_ufmp_inbox_root}/tests/financial_messaging/test_inbox_${_inbox_group}.c")
        target_link_libraries(${_inbox_target} PRIVATE Umicom::financial_inbox)
        set_target_properties(${_inbox_target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        umicom_apply_warnings(${_inbox_target})
        umicom_apply_sanitizers(${_inbox_target})
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(${_inbox_target})
        endif()
        if(COMMAND umicom_target_qualification_inputs)
            umicom_target_qualification_inputs(${_inbox_target} FILES
                "${_ufmp_inbox_root}/tests/financial_messaging/fixture.h")
        endif()
        foreach(_inbox_case IN LISTS _inbox_${_inbox_group}_cases)
            set(_inbox_test "framework.ufmp.inbox.${_inbox_group}.${_inbox_case}")
            if(_inbox_group STREQUAL "sqlite")
                add_test(NAME ${_inbox_test} COMMAND ${_inbox_target} ${_inbox_case}
                    "${CMAKE_CURRENT_BINARY_DIR}/ufmp-inbox-${_inbox_case}.sqlite3")
            else()
                add_test(NAME ${_inbox_test} COMMAND ${_inbox_target} ${_inbox_case})
            endif()
            set_tests_properties(${_inbox_test} PROPERTIES TIMEOUT 30 LABELS "framework;finance;ufmp;data;regression")
            if(COMMAND umicom_test_impact_owners)
                umicom_test_impact_owners(${_inbox_test} TARGETS ${_inbox_target})
            endif()
        endforeach()
    endforeach()
endif()
foreach(_inbox_variable IN ITEMS store_cases codec_cases sqlite_cases groups group target case test data_definitions)
    unset(_inbox_${_inbox_variable})
endforeach()
unset(_inbox_variable)
unset(_ufmp_inbox_root)

install(DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/../examples/financial_messaging/"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom-framework/examples/financial_messaging"
    COMPONENT Framework FILES_MATCHING PATTERN "*.c" PATTERN "CMakeLists.txt")
