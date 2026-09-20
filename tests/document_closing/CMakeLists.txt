#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/document_closing/CMakeLists.txt
# PURPOSE: Verify reviewed close using production documents and native bindings.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
add_executable(umicom-document-closing-test test_close_plan.c)
target_link_libraries(umicom-document-closing-test PRIVATE Umicom::document)
umicom_apply_warnings(umicom-document-closing-test)
umicom_apply_sanitizers(umicom-document-closing-test)
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-document-closing-test)
endif()
foreach(case IN ITEMS invalid missing summary clean save-clean cancel-clean cancel-dirty cancel-stale dirty-refused untitled-empty discard readonly-discard untitled-discard save untitled-save readonly-save path-limits invalid-decision changed-text history-cycle independent-save readonly-change renamed closed reopened view-missing other-active other-edits selection compacted external-conflict destination-owned write-failure destroy-owner large)
    add_test(NAME framework.document_closing.${case} COMMAND umicom-document-closing-test ${case})
    set_tests_properties(framework.document_closing.${case} PROPERTIES
        TIMEOUT 30 LABELS "framework;document;closing;regression")
endforeach()

if(TARGET umicom_ui_gtk4)
    add_executable(umicom-document-closing-gtk4-test test_close_gtk4.c)
    target_link_libraries(umicom-document-closing-gtk4-test PRIVATE Umicom::ui_gtk4 Umicom::document)
    umicom_apply_warnings(umicom-document-closing-gtk4-test)
    umicom_apply_sanitizers(umicom-document-closing-gtk4-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-document-closing-gtk4-test)
    endif()
    foreach(case clean cancel discard save stale other-tab unbind window-close complete-unbind tab-clean tab-pinned save-busy)
        add_test(NAME framework.document_closing.gtk4.${case} COMMAND umicom-document-closing-gtk4-test ${case})
        set_tests_properties(framework.document_closing.gtk4.${case} PROPERTIES TIMEOUT 30
            SKIP_RETURN_CODE 77 RUN_SERIAL TRUE LABELS "framework;document;closing;gtk4;native")
    endforeach()
endif()

# Build the exact source displayed in the public lesson.
add_executable(umicom-document-closing-example ../../examples/editor_workflow/close_document.c)
target_link_libraries(umicom-document-closing-example PRIVATE Umicom::document)
umicom_apply_warnings(umicom-document-closing-example)
umicom_apply_sanitizers(umicom-document-closing-example)
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-document-closing-example)
endif()
add_test(NAME framework.document_closing.example COMMAND umicom-document-closing-example)
set_tests_properties(framework.document_closing.example PROPERTIES
    TIMEOUT 30 LABELS "framework;document;closing;example")
install(TARGETS umicom-document-closing-example RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
