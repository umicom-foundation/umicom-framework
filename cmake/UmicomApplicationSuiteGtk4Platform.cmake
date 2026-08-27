#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomApplicationSuiteGtk4Platform.cmake
#
# PURPOSE:
#   Attach Application Suite layout hosting and generic view-model rendering to
#   the existing optional GTK4 adapter. No competing UI library is created.
#
# Created by: Sammy Hegab
# Organisation: Umicom Foundation
# Licence: MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)

set(UMICOM_APPLICATION_SUITE_GTK4_ROOT "${CMAKE_CURRENT_LIST_DIR}/..")

function(umicom_application_suite_attach_gtk4)
    if(NOT TARGET umicom_ui_gtk4)
        return()
    endif()
    get_target_property(
        _umicom_suite_gtk4_attached
        umicom_ui_gtk4
        UMICOM_APPLICATION_SUITE_GTK4_ATTACHED)
    if(_umicom_suite_gtk4_attached)
        return()
    endif()
    target_sources(umicom_ui_gtk4 PRIVATE
        "${UMICOM_APPLICATION_SUITE_GTK4_ROOT}/adapters/gtk4/workstation/workspace_layout_host_gtk4.c"
        "${UMICOM_APPLICATION_SUITE_GTK4_ROOT}/adapters/gtk4/workstation/view_model_panel_gtk4.c"
        "${UMICOM_APPLICATION_SUITE_GTK4_ROOT}/adapters/gtk4/application_suite_workstation_gtk4.c"
    )
    target_link_libraries(umicom_ui_gtk4 PUBLIC Umicom::application)
    set_property(
        TARGET umicom_ui_gtk4
        PROPERTY UMICOM_APPLICATION_SUITE_GTK4_ATTACHED TRUE)
endfunction()

umicom_application_suite_attach_gtk4()
if(NOT TARGET umicom_ui_gtk4 AND COMMAND cmake_language)
    cmake_language(DEFER CALL umicom_application_suite_attach_gtk4)
endif()
