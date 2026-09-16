#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomEditorWorkflowTests.cmake
# PURPOSE: Register working-copy and editor navigation regression tests.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
if(NOT BUILD_TESTING)
    return()
endif()
add_executable(umicom-document-editor-workflow-test
    "${CMAKE_CURRENT_LIST_DIR}/../tests/document/test_editor_workflow.c")
target_link_libraries(umicom-document-editor-workflow-test PRIVATE Umicom::Framework)
umicom_apply_warnings(umicom-document-editor-workflow-test)
umicom_apply_sanitizers(umicom-document-editor-workflow-test)
foreach(test_case IN ITEMS draft-snapshot draft-without-flag close-guard-draft
        virtual-draft reopen-draft reopen-missing reopen-alias
        new-small-output open-small-output view-capacity-rollback
        view-unterminated view-unterminated-preview view-id-collision view-field-boundaries replace-null replace-capacity replace-read-only
        nav-forward nav-backward nav-not-found nav-smart-case nav-utf8-offset
        nav-replace-selection nav-replace-advance nav-replace-delete nav-replace-all-undo
        nav-replace-all-noop nav-replace-all-capacity nav-read-only-history
        nav-close-captured nav-close-denied nav-reopen-small-output
        nav-close-guard-missing nav-close-guard-build nav-close-guard-dirty-build
        nav-save-clean-marker nav-history-capacity)
    set(test_name "framework.document.editor_workflow.${test_case}")
    add_test(NAME "${test_name}" COMMAND umicom-document-editor-workflow-test "${test_case}")
    set_tests_properties("${test_name}" PROPERTIES TIMEOUT 30 LABELS "framework;document;editor;regression")
endforeach()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-document-editor-workflow-test)
endif()

add_executable(umicom-editor-search-navigation-test
    "${CMAKE_CURRENT_LIST_DIR}/../tests/editor/test_search_navigation.c")
target_link_libraries(umicom-editor-search-navigation-test PRIVATE Umicom::editor)
umicom_apply_warnings(umicom-editor-search-navigation-test)
umicom_apply_sanitizers(umicom-editor-search-navigation-test)
foreach(test_case IN ITEMS directions wrap failure-preserves-output word-boundaries
        many-matches replace-capacity replace-empty replace-overlap replace-smart-case replace-no-match)
    set(test_name "framework.editor.search_navigation.${test_case}")
    add_test(NAME "${test_name}" COMMAND umicom-editor-search-navigation-test "${test_case}")
    set_tests_properties("${test_name}" PROPERTIES TIMEOUT 30 LABELS "framework;editor;regression")
endforeach()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-editor-search-navigation-test)
endif()
