# Umicom Framework
# PURPOSE: Share native library selection without duplicating persistence rules.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
include_guard(GLOBAL)
include(GNUInstallDirs)
get_filename_component(_umicom_workspace_library_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
add_library(umicom_workspace_library_gtk4 STATIC
    "${_umicom_workspace_library_root}/adapters/gtk4/workspace_library_file.c"
    "${_umicom_workspace_library_root}/adapters/gtk4/workspace_library_gtk4.c")
add_library(Umicom::workspace_library_gtk4 ALIAS umicom_workspace_library_gtk4)
set_target_properties(umicom_workspace_library_gtk4 PROPERTIES
    EXPORT_NAME workspace_library_gtk4 C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
target_include_directories(umicom_workspace_library_gtk4 PUBLIC
    "$<BUILD_INTERFACE:${_umicom_workspace_library_root}/include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
target_link_libraries(umicom_workspace_library_gtk4 PUBLIC Umicom::ui_gtk4 Umicom::data)
if(COMMAND umicom_apply_warnings)
    umicom_apply_warnings(umicom_workspace_library_gtk4)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umicom_workspace_library_gtk4)
endif()
install(TARGETS umicom_workspace_library_gtk4 EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}" COMPONENT Framework)
# Include this exported library surface in the opt-in SDK without changing full installs.
umicom_register_sdk_libraries(
    umicom_workspace_library_gtk4
)
install(FILES "${_umicom_workspace_library_root}/include/umicom/ui/gtk4/workspace_library.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/ui/gtk4" COMPONENT Framework)
# The portable workspace model already owns umicom-workspace-library-test.
# Keep the previous registration disabled for review; the native name below
# preserves these file and GTK checks alongside the portable model tests.
if(FALSE)
if(BUILD_TESTING)
    add_executable(umicom-workspace-library-test
        "${_umicom_workspace_library_root}/tests/workspace_library/test_library.c")
    target_link_libraries(umicom-workspace-library-test PRIVATE Umicom::workspace_library_gtk4)
    set_target_properties(umicom-workspace-library-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-workspace-library-test)
    endif()
    foreach(kind IN ITEMS files controls)
        add_test(NAME framework.workspace_library.${kind} COMMAND umicom-workspace-library-test ${kind})
        set_tests_properties(framework.workspace_library.${kind} PROPERTIES
            TIMEOUT 30 SKIP_RETURN_CODE 77 LABELS "framework;workspace;storage;regression")
    endforeach()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-workspace-library-test)
    endif()
endif()
endif()

if(BUILD_TESTING)
    add_executable(umicom-workspace-library-native-test
        "${_umicom_workspace_library_root}/tests/workspace_library/test_library.c")
    target_link_libraries(umicom-workspace-library-native-test PRIVATE Umicom::workspace_library_gtk4)
    set_target_properties(umicom-workspace-library-native-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-workspace-library-native-test)
    endif()
    foreach(kind IN ITEMS files controls)
        add_test(NAME framework.workspace_library.${kind} COMMAND umicom-workspace-library-native-test ${kind})
        set_tests_properties(framework.workspace_library.${kind} PROPERTIES
            TIMEOUT 30 SKIP_RETURN_CODE 77 LABELS "framework;workspace;storage;regression")
    endforeach()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-workspace-library-native-test)
    endif()
endif()

# Install the user workflow beside the Framework's other desktop guides.
install(FILES "${_umicom_workspace_library_root}/docs/guides/workspace-libraries.html"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/framework/docs/guides" COMPONENT Framework)

# Recent-location metadata uses the shared native file and catalogue contracts.
target_link_libraries(umicom_workspace_library_gtk4 PRIVATE Umicom::platform)

# Storage and widget regressions run separately so native metadata remains
# testable on machines without a graphical session.
if(BUILD_TESTING)
    foreach(kind IN ITEMS store gtk4)
        add_executable(umicom-library-history-${kind}-test
            "${_umicom_workspace_library_root}/tests/workspace_library/test_history_${kind}.c")
        target_link_libraries(umicom-library-history-${kind}-test PRIVATE Umicom::workspace_library_gtk4 Umicom::platform)
        set_target_properties(umicom-library-history-${kind}-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
        if(COMMAND umicom_apply_warnings)
            umicom_apply_warnings(umicom-library-history-${kind}-test)
        endif()
        if(COMMAND umicom_apply_sanitizers)
            umicom_apply_sanitizers(umicom-library-history-${kind}-test)
        endif()
        add_test(NAME framework.workspace_library.history.${kind} COMMAND umicom-library-history-${kind}-test)
        set_tests_properties(framework.workspace_library.history.${kind} PROPERTIES
            TIMEOUT 30 SKIP_RETURN_CODE 77 LABELS "framework;workspace;history;regression")
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(umicom-library-history-${kind}-test)
        endif()
    endforeach()
endif()
