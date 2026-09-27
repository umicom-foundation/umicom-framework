# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
include_guard(GLOBAL)
if(NOT TARGET Umicom::desktop_workspace)
    include("${CMAKE_CURRENT_LIST_DIR}/UmicomDesktopWorkspace.cmake")
endif()
if(NOT TARGET Umicom::ui_gtk4)
    message(FATAL_ERROR "Desktop workspace GTK requires the canonical Umicom::ui_gtk4 target")
endif()
add_library(umicom_desktop_workspace_gtk4 STATIC
    "${CMAKE_CURRENT_LIST_DIR}/../adapters/gtk4/desktop_workspace_gtk4.c")
add_library(Umicom::desktop_workspace_gtk4 ALIAS umicom_desktop_workspace_gtk4)
set_target_properties(umicom_desktop_workspace_gtk4 PROPERTIES EXPORT_NAME desktop_workspace_gtk4
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
target_link_libraries(umicom_desktop_workspace_gtk4 PUBLIC Umicom::desktop_workspace Umicom::ui_gtk4)
if(COMMAND umicom_apply_warnings)
    umicom_apply_warnings(umicom_desktop_workspace_gtk4)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umicom_desktop_workspace_gtk4)
endif()
install(TARGETS umicom_desktop_workspace_gtk4 EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Framework)
if(BUILD_TESTING)
    add_executable(umicom-desktop-workspace-gtk4-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/desktop_workspace/test_gtk4.c")
    target_link_libraries(umicom-desktop-workspace-gtk4-test PRIVATE Umicom::desktop_workspace_gtk4)
    set_target_properties(umicom-desktop-workspace-gtk4-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    foreach(_case IN ITEMS construct open-close close-during-open)
        add_test(NAME framework.desktop_workspace.gtk4.${_case}
            COMMAND umicom-desktop-workspace-gtk4-test ${_case})
        set_tests_properties(framework.desktop_workspace.gtk4.${_case} PROPERTIES
            TIMEOUT 30 SKIP_RETURN_CODE 77 LABELS "framework;desktop;workspace;gtk4;lifetime")
    endforeach()
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-desktop-workspace-gtk4-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-desktop-workspace-gtk4-test)
    endif()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-desktop-workspace-gtk4-test)
    endif()
endif()


# The reusable viewport and its component gallery supplement existing Desk UI.
# No prior widget, workspace entry point or saved-layout implementation is removed.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomWorkbenchViewportGtk4.cmake")
