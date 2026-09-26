#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomDeskFederation.cmake
# PURPOSE: Attach federation to the existing Desk runtime and optional GTK adapter.
# AUTHOR: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
include("${CMAKE_CURRENT_LIST_DIR}/UmicomDeskFederationCore.cmake")
set(_umi_desk_host_root "${CMAKE_CURRENT_LIST_DIR}/..")
if(NOT TARGET Umicom::Framework)
    message(FATAL_ERROR "Desk federation host requires the existing Framework runtime")
endif()
add_library(umicom_desk_federation_host STATIC
    "${_umi_desk_host_root}/src/desktop/federation/runtime_bridge.c")
add_library(Umicom::desk_federation_host ALIAS umicom_desk_federation_host)
set_target_properties(umicom_desk_federation_host PROPERTIES EXPORT_NAME desk_federation_host
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
target_link_libraries(umicom_desk_federation_host PUBLIC Umicom::desk_federation Umicom::Framework)
if(COMMAND umicom_apply_warnings)
    umicom_apply_warnings(umicom_desk_federation_host)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umicom_desk_federation_host)
endif()
install(TARGETS umicom_desk_federation_host EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Framework)
if(BUILD_TESTING)
    add_executable(umicom-desk-federation-runtime-test
        "${_umi_desk_host_root}/tests/desk_federation/test_runtime_bridge.c")
    target_link_libraries(umicom-desk-federation-runtime-test PRIVATE Umicom::desk_federation_host)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-desk-federation-runtime-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-desk-federation-runtime-test)
    endif()
    add_test(NAME framework.desk_federation.runtime_bridge COMMAND umicom-desk-federation-runtime-test)
    set_tests_properties(framework.desk_federation.runtime_bridge PROPERTIES
        TIMEOUT 60 LABELS "framework;desktop;desk-federation;integration")
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-desk-federation-runtime-test)
    endif()
endif()
if(TARGET Umicom::ui_gtk4)
    add_library(umicom_desk_federation_gtk4 STATIC
        "${_umi_desk_host_root}/adapters/gtk4/desk_federation_gtk4.c"
        "${_umi_desk_host_root}/adapters/gtk4/desk_federation_actions_gtk4.c")
    add_library(Umicom::desk_federation_gtk4 ALIAS umicom_desk_federation_gtk4)
    set_target_properties(umicom_desk_federation_gtk4 PROPERTIES EXPORT_NAME desk_federation_gtk4
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    target_link_libraries(umicom_desk_federation_gtk4 PUBLIC
        Umicom::desk_federation_host Umicom::ui_gtk4)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom_desk_federation_gtk4)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom_desk_federation_gtk4)
    endif()
    install(TARGETS umicom_desk_federation_gtk4 EXPORT UmicomFrameworkTargets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Framework)
    install(FILES "${_umi_desk_host_root}/include/umicom/ui/gtk4/desk_federation.h"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/ui/gtk4" COMPONENT Framework)
    if(BUILD_TESTING)
        add_executable(umicom-desk-federation-gtk-test
            "${_umi_desk_host_root}/tests/desk_federation/test_gtk_lifetime.c")
        target_link_libraries(umicom-desk-federation-gtk-test PRIVATE Umicom::desk_federation_gtk4)
        if(COMMAND umicom_apply_warnings)
            umicom_apply_warnings(umicom-desk-federation-gtk-test)
        endif()
        if(COMMAND umicom_apply_sanitizers)
            umicom_apply_sanitizers(umicom-desk-federation-gtk-test)
        endif()
        add_test(NAME framework.desk_federation.gtk_lifetime COMMAND umicom-desk-federation-gtk-test)
        set_tests_properties(framework.desk_federation.gtk_lifetime PROPERTIES TIMEOUT 60
            SKIP_RETURN_CODE 77 LABELS "framework;desktop;desk-federation;gtk4;integration")
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(umicom-desk-federation-gtk-test)
        endif()
    endif()
endif()
unset(_umi_desk_host_root)
