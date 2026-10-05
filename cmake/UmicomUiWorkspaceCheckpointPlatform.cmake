#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomUiWorkspaceCheckpointPlatform.cmake
# PURPOSE: Bind the native workspace codec to the existing Data Server store.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)

# This bridge sits above both UI and persistence. Neither low-level library
# acquires a reverse dependency on the other or on a product's GTK adapter.
add_library(umicom_ui_workspace_checkpoint STATIC
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/workspace_checkpoint.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/workspace_library_checkpoint.c")
add_library(Umicom::ui_workspace_checkpoint ALIAS umicom_ui_workspace_checkpoint)
set_target_properties(umicom_ui_workspace_checkpoint PROPERTIES
    EXPORT_NAME ui_workspace_checkpoint)
target_include_directories(umicom_ui_workspace_checkpoint PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/../include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
target_link_libraries(umicom_ui_workspace_checkpoint PUBLIC
    Umicom::ui Umicom::workbench_layout_data Umicom::application Umicom::document)
if(COMMAND umicom_apply_warnings)
    umicom_apply_warnings(umicom_ui_workspace_checkpoint)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umicom_ui_workspace_checkpoint)
endif()
if(TARGET umicom_framework)
    target_link_libraries(umicom_framework INTERFACE Umicom::ui_workspace_checkpoint)
endif()
if(TARGET umicom_ui_gtk4)
    # Shared native hosts call this bridge directly, without relying on the
    # complete Framework aggregate to supply an accidental link dependency.
    target_link_libraries(umicom_ui_gtk4 PUBLIC Umicom::ui_workspace_checkpoint)
endif()

if(BUILD_TESTING)
    # The complete named library shares the same Data Server bridge. Its
    # regression remains independent of any graphical product executable.
    add_executable(umicom-ui-workspace-library-checkpoint-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ui/test_workspace_library_checkpoint.c")
    target_link_libraries(umicom-ui-workspace-library-checkpoint-test PRIVATE
        Umicom::ui_workspace_checkpoint)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ui-workspace-library-checkpoint-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ui-workspace-library-checkpoint-test)
    endif()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-ui-workspace-library-checkpoint-test)
    endif()
    add_test(NAME framework.ui.workspace_library_checkpoint
        COMMAND umicom-ui-workspace-library-checkpoint-test)
    set_tests_properties(framework.ui.workspace_library_checkpoint PROPERTIES
        LABELS "framework;ui;workspace;library;persistence;regression")

    add_executable(umicom-ui-workspace-checkpoint-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ui/test_workspace_checkpoint.c")
    target_link_libraries(umicom-ui-workspace-checkpoint-test PRIVATE
        Umicom::ui_workspace_checkpoint)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ui-workspace-checkpoint-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ui-workspace-checkpoint-test)
    endif()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-ui-workspace-checkpoint-test)
    endif()
    add_test(NAME framework.ui.workspace-checkpoint
        COMMAND umicom-ui-workspace-checkpoint-test)
    set_tests_properties(framework.ui.workspace-checkpoint PROPERTIES
        LABELS "framework;ui;workspace;canvas;persistence;regression")
endif()

install(TARGETS umicom_ui_workspace_checkpoint
    EXPORT UmicomFrameworkTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# Exercise file exchange without GTK, storage or application side effects.
if(BUILD_TESTING)
    add_executable(umicom-ui-workspace-library-exchange-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ui/test_workspace_library_exchange.c")
    target_link_libraries(umicom-ui-workspace-library-exchange-test PRIVATE Umicom::ui_workspace_checkpoint)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ui-workspace-library-exchange-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ui-workspace-library-exchange-test)
    endif()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-ui-workspace-library-exchange-test)
    endif()
    add_test(NAME framework.ui.workspace_library_exchange COMMAND umicom-ui-workspace-library-exchange-test)
    set_tests_properties(framework.ui.workspace_library_exchange PROPERTIES
        LABELS "framework;ui;workspace;import;regression" TIMEOUT 60)
endif()

# History stages canonical archives before native publication, so the bridge
# owns recovery without moving widget ownership into the portable UI model.
target_sources(umicom_ui_workspace_checkpoint PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/ui/workspace_library_history.c")
if(BUILD_TESTING)
    add_executable(umicom-ui-workspace-library-history-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ui/test_workspace_library_history.c")
    target_link_libraries(umicom-ui-workspace-library-history-test PRIVATE Umicom::ui_workspace_checkpoint)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ui-workspace-library-history-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ui-workspace-library-history-test)
    endif()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-ui-workspace-library-history-test)
    endif()
    add_test(NAME framework.ui.workspace_library_history COMMAND umicom-ui-workspace-library-history-test)
    set_tests_properties(framework.ui.workspace_library_history PROPERTIES
        LABELS "framework;ui;workspace;history;regression" TIMEOUT 60)
endif()

# Recovery scenarios share production storage and ownership boundaries.
if(BUILD_TESTING)
    add_executable(umicom-saved-library-review-test "${CMAKE_CURRENT_LIST_DIR}/../tests/ui/test_saved_library_review.c")
    target_link_libraries(umicom-saved-library-review-test PRIVATE Umicom::ui_workspace_checkpoint)
    set_target_properties(umicom-saved-library-review-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-saved-library-review-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-saved-library-review-test)
    endif()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-saved-library-review-test)
    endif()
    foreach(case IN ITEMS evidence frozen stale foreign recovery missing transaction invalid)
        add_test(NAME framework.ui.saved_library_review.${case} COMMAND umicom-saved-library-review-test ${case})
        set_tests_properties(framework.ui.saved_library_review.${case} PROPERTIES TIMEOUT 30 LABELS "framework;workspace;recovery;regression")
    endforeach()
endif()
