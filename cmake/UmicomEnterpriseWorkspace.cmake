#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomEnterpriseWorkspace.cmake
# PURPOSE: Compose shared reference-data imports without duplicating product logic.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
include(GNUInstallDirs)
include(CMakeParseArguments)
set(_umicom_enterprise_root "${CMAKE_CURRENT_LIST_DIR}/..")
foreach(dependency IN ITEMS Umicom::data Umicom::security)
    if(NOT TARGET ${dependency})
        message(FATAL_ERROR "Enterprise workspace requires canonical ${dependency}")
    endif()
endforeach()
function(umicom_enterprise_configure_target target)
    set_target_properties(${target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(${target})
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(${target})
    endif()
endfunction()
add_library(umicom_enterprise_workspace STATIC
    "${_umicom_enterprise_root}/src/enterprise_workspace/text.c"
    "${_umicom_enterprise_root}/src/enterprise_workspace/csv.c"
    "${_umicom_enterprise_root}/src/enterprise_workspace/wire.c"
    "${_umicom_enterprise_root}/src/enterprise_workspace/storage.c"
    "${_umicom_enterprise_root}/src/enterprise_workspace/validation.c"
    "${_umicom_enterprise_root}/src/enterprise_workspace/workspace.c"
    "${_umicom_enterprise_root}/src/enterprise_workspace/practice.c")
add_library(Umicom::enterprise_workspace ALIAS umicom_enterprise_workspace)
set_target_properties(umicom_enterprise_workspace PROPERTIES EXPORT_NAME enterprise_workspace)
target_include_directories(umicom_enterprise_workspace PUBLIC
    $<BUILD_INTERFACE:${_umicom_enterprise_root}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(umicom_enterprise_workspace PUBLIC Umicom::data Umicom::security)
umicom_enterprise_configure_target(umicom_enterprise_workspace)
install(TARGETS umicom_enterprise_workspace EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT Framework)
install(DIRECTORY "${_umicom_enterprise_root}/include/umicom/enterprise_workspace"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom" COMPONENT Framework)
if(TARGET Umicom::ui_gtk4)
    add_library(umicom_enterprise_workspace_gtk4 STATIC
        "${_umicom_enterprise_root}/adapters/gtk4/enterprise_workspace_panel_gtk4.c"
        "${_umicom_enterprise_root}/adapters/gtk4/enterprise_workspace_application_gtk4.c")
    add_library(Umicom::enterprise_workspace_gtk4 ALIAS umicom_enterprise_workspace_gtk4)
    set_target_properties(umicom_enterprise_workspace_gtk4 PROPERTIES EXPORT_NAME enterprise_workspace_gtk4)
    target_link_libraries(umicom_enterprise_workspace_gtk4 PUBLIC Umicom::enterprise_workspace Umicom::ui_gtk4)
    umicom_enterprise_configure_target(umicom_enterprise_workspace_gtk4)
    install(TARGETS umicom_enterprise_workspace_gtk4 EXPORT UmicomFrameworkTargets
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT Framework)
    install(FILES "${_umicom_enterprise_root}/include/umicom/ui/gtk4/enterprise_workspace.h"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/ui/gtk4" COMPONENT Framework)
endif()
if(BUILD_TESTING)
    add_executable(umicom-enterprise-workspace-test "${_umicom_enterprise_root}/tests/enterprise_workspace/test_workspace.c")
    target_include_directories(umicom-enterprise-workspace-test PRIVATE "${_umicom_enterprise_root}/src/enterprise_workspace")
    target_link_libraries(umicom-enterprise-workspace-test PRIVATE Umicom::enterprise_workspace)
    umicom_enterprise_configure_target(umicom-enterprise-workspace-test)
    include("${_umicom_enterprise_root}/tests/enterprise_workspace/cases.cmake")
    foreach(case IN LISTS UMICOM_ENTERPRISE_WORKSPACE_CASES)
        add_test(NAME "framework.enterprise_workspace.${case}" COMMAND umicom-enterprise-workspace-test "${case}"
            "${CMAKE_CURRENT_BINARY_DIR}/enterprise-${case}.sqlite")
        set_tests_properties("framework.enterprise_workspace.${case}" PROPERTIES TIMEOUT 30 SKIP_RETURN_CODE 77
            LABELS "framework;enterprise-workspace;regression")
    endforeach()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-enterprise-workspace-test)
    endif()
    if(TARGET Umicom::enterprise_workspace_gtk4)
        add_executable(umicom-enterprise-workspace-gtk-test "${_umicom_enterprise_root}/tests/enterprise_workspace/test_panel_gtk4.c")
        target_link_libraries(umicom-enterprise-workspace-gtk-test PRIVATE Umicom::enterprise_workspace_gtk4)
        umicom_enterprise_configure_target(umicom-enterprise-workspace-gtk-test)
        foreach(case workflow retained_controls)
            add_test(NAME "framework.enterprise_workspace.gtk.${case}" COMMAND umicom-enterprise-workspace-gtk-test "${case}")
            set_tests_properties("framework.enterprise_workspace.gtk.${case}" PROPERTIES TIMEOUT 30 SKIP_RETURN_CODE 77
                LABELS "framework;enterprise-workspace;gtk4;regression")
        endforeach()
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(umicom-enterprise-workspace-gtk-test)
        endif()
    endif()
endif()
# Products attach one capability. Their existing sources, consoles and tests
# remain active; the parent uses this property only for the new native surface.
function(umicom_attach_enterprise_workspace)
    cmake_parse_arguments(PARSE_ARGV 0 UMI "" "NAME;TARGET" "")
    if(UMI_UNPARSED_ARGUMENTS OR NOT UMI_NAME OR NOT UMI_TARGET OR NOT TARGET "${UMI_TARGET}")
        message(FATAL_ERROR "Enterprise attachment requires NAME and an existing TARGET")
    endif()
    get_target_property(attached "${UMI_TARGET}" UMICOM_ENTERPRISE_WORKSPACE_ATTACHED)
    if(attached)
        message(FATAL_ERROR "Enterprise workspace already attached to ${UMI_TARGET}")
    endif()
    set_target_properties("${UMI_TARGET}" PROPERTIES UMICOM_ENTERPRISE_WORKSPACE_ATTACHED TRUE)
    target_link_libraries("${UMI_TARGET}" PUBLIC Umicom::enterprise_workspace)
    set(example "umicom-${UMI_NAME}-enterprise-workspace-example")
    add_executable("${example}" "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../examples/enterprise_workspace_main.c")
    target_link_libraries("${example}" PRIVATE Umicom::enterprise_workspace)
    umicom_enterprise_configure_target("${example}")
    install(TARGETS "${example}" RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
    if(BUILD_TESTING)
        add_test(NAME "${UMI_NAME}.enterprise_workspace.example" COMMAND "${example}")
        set_tests_properties("${UMI_NAME}.enterprise_workspace.example" PROPERTIES TIMEOUT 30 LABELS "enterprise-workspace;example")
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target("${example}")
        endif()
    endif()
endfunction()
install(FILES "${_umicom_enterprise_root}/docs/learning/ENTERPRISE_WORKSPACE.html"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/docs/learning" COMPONENT Framework)
unset(_umicom_enterprise_root)
