#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomCreativeWorkspace.cmake
# PURPOSE: Compose shared creative services and thin native product profiles.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
include(GNUInstallDirs)
include(CMakeParseArguments)
set(_umicom_creative_root "${CMAKE_CURRENT_LIST_DIR}/..")
foreach(dependency IN ITEMS Umicom::data Umicom::media_engine)
    if(NOT TARGET ${dependency})
        message(FATAL_ERROR "Creative workspace requires canonical ${dependency}")
    endif()
endforeach()
function(umicom_creative_configure_target target)
    set_target_properties(${target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(${target})
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(${target})
    endif()
endfunction()
add_library(umicom_creative_workspace STATIC
    "${_umicom_creative_root}/src/creative_workspace/brand_assets.c"
    "${_umicom_creative_root}/src/creative_workspace/export_file.c"
    "${_umicom_creative_root}/src/creative_workspace/export_html.c"
    "${_umicom_creative_root}/src/creative_workspace/export_svg.c"
    "${_umicom_creative_root}/src/creative_workspace/export_wave.c"
    "${_umicom_creative_root}/src/creative_workspace/export_writer.c"
    "${_umicom_creative_root}/src/creative_workspace/motion.c"
    "${_umicom_creative_root}/src/creative_workspace/project.c"
    "${_umicom_creative_root}/src/creative_workspace/scene.c"
    "${_umicom_creative_root}/src/creative_workspace/starters.c"
    "${_umicom_creative_root}/src/creative_workspace/storage.c"
    "${_umicom_creative_root}/src/creative_workspace/text.c"
    "${_umicom_creative_root}/src/creative_workspace/validation.c"
    "${_umicom_creative_root}/src/creative_workspace/wire.c"
    "${_umicom_creative_root}/src/creative_workspace/workspace.c"
 )
add_library(Umicom::creative_workspace ALIAS umicom_creative_workspace)
set_target_properties(umicom_creative_workspace PROPERTIES EXPORT_NAME creative_workspace)
target_include_directories(umicom_creative_workspace PUBLIC
    $<BUILD_INTERFACE:${_umicom_creative_root}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(umicom_creative_workspace PUBLIC Umicom::data Umicom::media_engine)
if(NOT WIN32)
    target_link_libraries(umicom_creative_workspace PUBLIC m)
endif()
umicom_creative_configure_target(umicom_creative_workspace)
install(TARGETS umicom_creative_workspace EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT Framework)
install(DIRECTORY "${_umicom_creative_root}/include/umicom/creative_workspace"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom" COMPONENT Framework)
if(TARGET Umicom::ui_gtk4)
    add_library(umicom_creative_workspace_gtk4 STATIC
        "${_umicom_creative_root}/adapters/gtk4/creative_workspace_panel_gtk4.c"
        "${_umicom_creative_root}/adapters/gtk4/creative_workspace_commands_gtk4.c"
        "${_umicom_creative_root}/adapters/gtk4/creative_workspace_canvas_gtk4.c"
        "${_umicom_creative_root}/adapters/gtk4/creative_workspace_surface_gtk4.c"
        "${_umicom_creative_root}/adapters/gtk4/creative_workspace_application_gtk4.c")
    add_library(Umicom::creative_workspace_gtk4 ALIAS umicom_creative_workspace_gtk4)
    set_target_properties(umicom_creative_workspace_gtk4 PROPERTIES EXPORT_NAME creative_workspace_gtk4)
    target_link_libraries(umicom_creative_workspace_gtk4 PUBLIC Umicom::creative_workspace Umicom::ui_gtk4)
    umicom_creative_configure_target(umicom_creative_workspace_gtk4)
    install(TARGETS umicom_creative_workspace_gtk4 EXPORT UmicomFrameworkTargets
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT Framework)
    install(FILES "${_umicom_creative_root}/include/umicom/ui/gtk4/creative_workspace.h"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/ui/gtk4" COMPONENT Framework)
endif()
if(BUILD_TESTING)
    add_executable(umicom-creative-workspace-test
        "${_umicom_creative_root}/tests/creative_workspace/test_creative_workspace.c")
    target_include_directories(umicom-creative-workspace-test PRIVATE
        "${_umicom_creative_root}/src/creative_workspace")
    target_link_libraries(umicom-creative-workspace-test PRIVATE Umicom::creative_workspace)
    umicom_creative_configure_target(umicom-creative-workspace-test)
    include("${_umicom_creative_root}/tests/creative_workspace/cases.cmake")
    foreach(case IN LISTS UMICOM_CREATIVE_WORKSPACE_CASES)
        file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/creative-tests/${case}")
        add_test(NAME "framework.creative_workspace.${case}"
            COMMAND umicom-creative-workspace-test "${case}")
        set_tests_properties("framework.creative_workspace.${case}" PROPERTIES
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/creative-tests/${case}"
            TIMEOUT 30 SKIP_RETURN_CODE 77 LABELS "framework;creative-workspace;regression")
    endforeach()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-creative-workspace-test)
    endif()
    add_executable(umicom-creative-export-fixture "${_umicom_creative_root}/tests/creative_workspace/export_fixture.c")
    target_link_libraries(umicom-creative-export-fixture PRIVATE Umicom::creative_workspace)
    umicom_creative_configure_target(umicom-creative-export-fixture)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-creative-export-fixture)
    endif()
    find_package(Python3 COMPONENTS Interpreter QUIET)
    if(Python3_Interpreter_FOUND)
        add_test(NAME framework.creative_workspace.independent_exports
            COMMAND "${Python3_EXECUTABLE}" "${_umicom_creative_root}/tests/creative_workspace/check_exports.py"
                $<TARGET_FILE:umicom-creative-export-fixture>)
        set_tests_properties(framework.creative_workspace.independent_exports PROPERTIES TIMEOUT 60 LABELS "creative-workspace;export;regression")
    else()
        message(STATUS "Creative workspace independent export check not registered: Python3 interpreter unavailable")
    endif()
    if(TARGET Umicom::creative_workspace_gtk4)
        add_executable(umicom-creative-workspace-gtk-test
            "${_umicom_creative_root}/tests/creative_workspace/test_panel_gtk4.c")
        target_link_libraries(umicom-creative-workspace-gtk-test PRIVATE Umicom::creative_workspace_gtk4)
        umicom_creative_configure_target(umicom-creative-workspace-gtk-test)
        foreach(case workflow retained_controls surface_lifetime)
            add_test(NAME "framework.creative_workspace.gtk.${case}"
                COMMAND umicom-creative-workspace-gtk-test "${case}")
            set_tests_properties("framework.creative_workspace.gtk.${case}" PROPERTIES
                TIMEOUT 30 SKIP_RETURN_CODE 77 LABELS "framework;creative-workspace;gtk4;regression")
        endforeach()
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(umicom-creative-workspace-gtk-test)
        endif()
    endif()
endif()
# A product selects a profile rather than copying any creative service.
function(umicom_attach_creative_workspace)
    cmake_parse_arguments(PARSE_ARGV 0 UMI "" "NAME;TARGET" "")
    if(UMI_UNPARSED_ARGUMENTS OR NOT UMI_NAME OR NOT UMI_TARGET OR NOT TARGET "${UMI_TARGET}")
        message(FATAL_ERROR "Creative attachment requires NAME and an existing TARGET")
    endif()
    if(NOT UMI_NAME MATCHES "^(media|music|cad|kitchen|games|web-studio|mobile-studio)$")
        message(FATAL_ERROR "Unknown creative profile: ${UMI_NAME}")
    endif()
    get_target_property(attached "${UMI_TARGET}" UMICOM_CREATIVE_WORKSPACE_ATTACHED)
    if(attached)
        message(FATAL_ERROR "Creative workspace already attached to ${UMI_TARGET}")
    endif()
    set_target_properties("${UMI_TARGET}" PROPERTIES
        UMICOM_CREATIVE_WORKSPACE_ATTACHED TRUE UMICOM_CREATIVE_WORKSPACE_PROFILE "${UMI_NAME}")
    target_link_libraries("${UMI_TARGET}" PUBLIC Umicom::creative_workspace)
    set(example "umicom-${UMI_NAME}-creative-workspace-example")
    add_executable("${example}" "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../examples/creative_workspace_main.c")
    target_compile_definitions("${example}" PRIVATE UMICOM_CREATIVE_PROFILE="${UMI_NAME}")
    target_link_libraries("${example}" PRIVATE Umicom::creative_workspace)
    umicom_creative_configure_target("${example}")
    install(TARGETS "${example}" RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
    if(BUILD_TESTING)
        add_test(NAME "${UMI_NAME}.creative_workspace.example" COMMAND "${example}")
        set_tests_properties("${UMI_NAME}.creative_workspace.example" PROPERTIES
            TIMEOUT 30 LABELS "creative-workspace;example")
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target("${example}")
        endif()
    endif()
endfunction()
install(FILES "${_umicom_creative_root}/docs/learning/CREATIVE_WORKSPACE.html"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/docs/learning" COMPONENT Framework)
unset(_umicom_creative_root)
