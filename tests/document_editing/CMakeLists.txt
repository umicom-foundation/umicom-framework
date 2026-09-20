#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/document_editing/CMakeLists.txt
# PURPOSE: Register checks against the real document library and GTK adapter.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
add_executable(umicom-document-editing-test test_document_editing.c)
target_link_libraries(umicom-document-editing-test PRIVATE Umicom::document)
umicom_apply_warnings(umicom-document-editing-test)
umicom_apply_sanitizers(umicom-document-editing-test)
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-document-editing-test)
endif()
foreach(case IN ITEMS
    command-identities invalid-input initial-state select-all readonly-selection
    pending-undo sync-group redo-new-edit large-tail maximum-size utf8-selection
    utf8-boundary utf8-history empty-insert untitled copy-owned readonly-copy replace-undo
    delete-undo other-tab same-text consumed malformed-utf8 embedded-zero
    oversized-input readonly-apply closed-target wrong-owner stale-caret
    stale-selection stale-draft stale-store stale-save stale-save-as)
    add_test(NAME framework.document_editing.${case} COMMAND umicom-document-editing-test ${case})
    set_tests_properties(framework.document_editing.${case} PROPERTIES TIMEOUT 30
        LABELS "framework;document;editing;regression")
endforeach()

# Compile the exact source embedded in the public lesson and Wiki.
add_executable(umicom-document-editing-example ../../examples/editor_workflow/edit_commands.c)
target_link_libraries(umicom-document-editing-example PRIVATE Umicom::document)
umicom_apply_warnings(umicom-document-editing-example)
umicom_apply_sanitizers(umicom-document-editing-example)
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-document-editing-example)
endif()
add_test(NAME framework.document_editing.example COMMAND umicom-document-editing-example)
set_tests_properties(framework.document_editing.example PROPERTIES TIMEOUT 30
    LABELS "framework;document;editing;example")
install(TARGETS umicom-document-editing-example RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

if(TARGET umicom_ui_gtk4)
    add_executable(umicom-document-editing-gtk4-test test_document_editing_gtk4.c)
    target_link_libraries(umicom-document-editing-gtk4-test PRIVATE Umicom::ui_gtk4 Umicom::document)
    umicom_apply_warnings(umicom-document-editing-gtk4-test)
    umicom_apply_sanitizers(umicom-document-editing-gtk4-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-document-editing-gtk4-test)
    endif()
    foreach(case clipboard stale-draft other-tab closed readonly unbind cut-detach source-signals)
        add_test(NAME framework.document_editing.gtk4.${case} COMMAND umicom-document-editing-gtk4-test ${case})
        set_tests_properties(framework.document_editing.gtk4.${case} PROPERTIES
            TIMEOUT 30 SKIP_RETURN_CODE 77 RUN_SERIAL TRUE LABELS "framework;document;gtk4;native;editing")
    endforeach()
endif()


if(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    add_executable(umicom-document-editing-allocation-test test_document_editing.c)
    target_compile_definitions(umicom-document-editing-allocation-test PRIVATE UMICOM_EDIT_ALLOCATION_TEST=1)
    target_link_libraries(umicom-document-editing-allocation-test PRIVATE Umicom::document)
    target_link_options(umicom-document-editing-allocation-test PRIVATE
        "-Wl,--wrap=malloc" "-Wl,--wrap=calloc" "-Wl,--wrap=realloc")
    umicom_apply_warnings(umicom-document-editing-allocation-test)
    umicom_apply_sanitizers(umicom-document-editing-allocation-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-document-editing-allocation-test)
    endif()
    add_test(NAME framework.document_editing.allocation COMMAND umicom-document-editing-allocation-test allocation)
    set_tests_properties(framework.document_editing.allocation PROPERTIES TIMEOUT 30
        LABELS "framework;document;editing;fault-injection")
endif()
