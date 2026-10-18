# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Teaching-file UI uses the same SDK parsers as the command and installed example.
# Retained for review: guarding before the availability check prevented a later
# GTK-enabled consumer from retrying this module. The guard below records only
# an inclusion that can actually create the provider.
# This module also supports independent composition; own its SDK registration dependency.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomSdkComponent.cmake")
if(FALSE)
    include_guard(GLOBAL)
endif()
if(NOT TARGET Umicom::ui_gtk4)
    return()
endif()
include_guard(GLOBAL)
# Focused learning builds must name the same parser and path-picker owners.
include("${CMAKE_CURRENT_LIST_DIR}/UmicomEducationInspection.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/UmicomPathPicker.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/UmicomTextExport.cmake")
get_filename_component(_education_inspection_ui_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
add_library(umicom_education_inspection_gtk4 STATIC
    "${_education_inspection_ui_root}/adapters/gtk4/education_inspection_gtk4.c")
add_library(Umicom::education_inspection_gtk4 ALIAS umicom_education_inspection_gtk4)
set_target_properties(umicom_education_inspection_gtk4 PROPERTIES EXPORT_NAME education_inspection_gtk4)
target_link_libraries(umicom_education_inspection_gtk4 PUBLIC Umicom::ui_gtk4
    PRIVATE Umicom::education_inspection Umicom::platform)
umicom_education_inspection_configure_target(umicom_education_inspection_gtk4)
install(TARGETS umicom_education_inspection_gtk4 EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT Framework)
# Include this exported library surface in the opt-in SDK without changing full installs.
umicom_register_sdk_libraries(
    umicom_education_inspection_gtk4
)
install(FILES "${_education_inspection_ui_root}/include/umicom/ui/gtk4/education_inspection.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/ui/gtk4" COMPONENT Framework)
# Retained for review: this reverse attachment depended on the consumer already
# existing. UmicomEducationWorkspace.cmake now declares its own dependency after
# including this provider, so both inclusion orders retain the inspection UI.
if(FALSE)
    if(TARGET umicom_education_workspace_gtk4)
        target_link_libraries(umicom_education_workspace_gtk4 PRIVATE Umicom::education_inspection_gtk4)
    endif()
endif()
if(BUILD_TESTING)
    add_executable(umicom-education-inspection-ui-test
        "${_education_inspection_ui_root}/tests/education/test_inspection_gtk4.c")
    target_link_libraries(umicom-education-inspection-ui-test PRIVATE Umicom::education_inspection_gtk4)
    umicom_education_inspection_configure_target(umicom-education-inspection-ui-test)
    add_test(NAME framework.education.inspection.gtk4 COMMAND umicom-education-inspection-ui-test)
    set_tests_properties(framework.education.inspection.gtk4 PROPERTIES
        SKIP_RETURN_CODE 77 TIMEOUT 30 LABELS "framework;education;gtk4;inspection;regression")
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-education-inspection-ui-test)
    endif()
endif()
