# Umicom Framework
# File: cmake/UmicomLocalFileDrop.cmake
# PURPOSE: Add native file-drop ownership to the shared GTK service.
# AUTHOR AND ORGANISATION: Sammy Hegab / Umicom Foundation
# LICENCE: MIT
if(NOT TARGET umicom_ui_gtk4)
    return()
endif()
include_guard(GLOBAL)
include(GNUInstallDirs)
get_filename_component(_local_drop_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
target_sources(umicom_ui_gtk4 PRIVATE "${_local_drop_root}/adapters/gtk4/local_file_drop_gtk4.c")
install(FILES "${_local_drop_root}/include/umicom/ui/gtk4/local_file_drop.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/ui/gtk4" COMPONENT Framework)
if(COMMAND umicom_release_inventory_own_headers)
    umicom_release_inventory_own_headers(umicom_ui_gtk4 include/umicom/ui/gtk4/local_file_drop.h)
endif()

# Keep the guide with the Framework-owned public header in installed SDKs.
foreach(_local_drop_component IN ITEMS Framework FrameworkSDK)
    set(_local_drop_exclude "")
    if(_local_drop_component STREQUAL "FrameworkSDK")
        set(_local_drop_exclude EXCLUDE_FROM_ALL)
    endif()
    install(FILES "${_local_drop_root}/docs/guides/local-file-drops.html"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom-framework/docs/guides"
        COMPONENT ${_local_drop_component} ${_local_drop_exclude})
endforeach()
unset(_local_drop_component)
unset(_local_drop_exclude)

if(BUILD_TESTING)
    add_executable(umicom-local-file-drop-test "${_local_drop_root}/tests/local_file_drop/test_local_file_drop.c")
    target_link_libraries(umicom-local-file-drop-test PRIVATE Umicom::ui_gtk4)
    set_target_properties(umicom-local-file-drop-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umicom-local-file-drop-test)
    umicom_apply_sanitizers(umicom-local-file-drop-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-local-file-drop-test)
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-local-file-drop-test FILES
            "${_local_drop_root}/tests/support/main_context.h")
    endif()
    foreach(_local_drop_case IN ITEMS local duplicate limit remote mixed-remote long wrong-type
            null-list unparented hidden insensitive reentry remove close clear-value receiver-error constructor)
        add_test(NAME framework.local_files.drop.gtk4.${_local_drop_case}
            COMMAND umicom-local-file-drop-test ${_local_drop_case})
        set_tests_properties(framework.local_files.drop.gtk4.${_local_drop_case} PROPERTIES
            TIMEOUT 30 SKIP_RETURN_CODE 77 ENVIRONMENT "G_DEBUG=fatal-warnings"
            LABELS "framework;gtk4;files;ownership;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(framework.local_files.drop.gtk4.${_local_drop_case}
                TARGETS umicom-local-file-drop-test)
        endif()
    endforeach()
endif()
unset(_local_drop_root)
unset(_local_drop_case)
