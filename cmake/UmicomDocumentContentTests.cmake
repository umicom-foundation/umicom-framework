#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomDocumentContentTests.cmake
# PURPOSE: Register complete-draft and source-file editing regression tests.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)
if(NOT BUILD_TESTING)
    return()
endif()
add_executable(umicom-document-content-test
    "${CMAKE_CURRENT_LIST_DIR}/../tests/ui/test_document_content.c")
target_link_libraries(umicom-document-content-test PRIVATE Umicom::ui)
umicom_apply_warnings(umicom-document-content-test)
umicom_apply_sanitizers(umicom-document-content-test)
foreach(test_case IN ITEMS complete-copy metadata legacy-edit-rejected explicit-shrink
        tail-revision reorder preview-reuse invalid-input reserve owned-copy
        new-identity utf8-preview maximum budget-release concurrent-copies)
    add_test(NAME "framework.document_content.${test_case}"
        COMMAND umicom-document-content-test "${test_case}")
    set_tests_properties("framework.document_content.${test_case}" PROPERTIES
        TIMEOUT 60 LABELS "framework;document-content;editor;regression")
endforeach()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-document-content-test)
endif()

add_executable(umicom-document-source-files-test
    "${CMAKE_CURRENT_LIST_DIR}/../tests/document/test_source_files.c")
target_link_libraries(umicom-document-source-files-test PRIVATE Umicom::Framework)
umicom_apply_warnings(umicom-document-source-files-test)
umicom_apply_sanitizers(umicom-document-source-files-test)
foreach(test_case IN ITEMS open-complete tail-search tail-dirty save-complete
        reopen-draft replace-history many-replacements replace-rejected read-only
        external-conflict captured-save history-budget shrink-and-undo
        import-large import-rollback raw-limit decoded-limit
        loader-preflight loader-no-stat loader-changed-size loader-stat-error)
    add_test(NAME "framework.document_content.workflow.${test_case}"
        COMMAND umicom-document-source-files-test "${test_case}")
    set_tests_properties("framework.document_content.workflow.${test_case}" PROPERTIES
        TIMEOUT 60 LABELS "framework;document-content;editor;regression")
endforeach()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-document-source-files-test)
endif()
