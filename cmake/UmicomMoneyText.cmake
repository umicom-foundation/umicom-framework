#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomMoneyText.cmake
# PURPOSE: Build one toolkit-neutral canonical money formatter.
# AUTHOR: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
if(TARGET Umicom::money_text)
    return()
endif()
include(GNUInstallDirs)
set(_umi_money_root "${CMAKE_CURRENT_LIST_DIR}/..")
if(NOT TARGET Umicom::finance)
    message(FATAL_ERROR "Umicom money text requires canonical Umicom::finance")
endif()
add_library(umicom_money_text STATIC "${_umi_money_root}/src/finance/money_text.c")
add_library(Umicom::money_text ALIAS umicom_money_text)
set_target_properties(umicom_money_text PROPERTIES EXPORT_NAME money_text
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
target_include_directories(umicom_money_text PUBLIC
    $<BUILD_INTERFACE:${_umi_money_root}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(umicom_money_text PUBLIC Umicom::finance)
if(COMMAND umicom_apply_warnings)
    umicom_apply_warnings(umicom_money_text)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umicom_money_text)
endif()
install(TARGETS umicom_money_text EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Framework)
install(FILES "${_umi_money_root}/include/umicom/finance/money_text.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/finance" COMPONENT Framework)
if(BUILD_TESTING)
    add_executable(umicom-money-text-test "${_umi_money_root}/tests/finance/test_money_text.c")
    target_link_libraries(umicom-money-text-test PRIVATE Umicom::money_text)
    set_target_properties(umicom-money-text-test PROPERTIES C_STANDARD 23
        C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-money-text-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-money-text-test)
    endif()
    foreach(case IN ITEMS amounts extremes all_scales capacity invalid arguments overlapping)
        add_test(NAME framework.money_text.${case} COMMAND umicom-money-text-test "${case}")
        set_tests_properties(framework.money_text.${case} PROPERTIES TIMEOUT 30
            LABELS "framework;finance;money-text;regression")
    endforeach()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-money-text-test)
    endif()
endif()
unset(_umi_money_root)
