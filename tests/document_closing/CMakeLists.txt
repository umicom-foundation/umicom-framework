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

# Closing several sources reuses the reviewed single-document contract.
add_executable(umicom-document-close-session-test test_close_session.c)
target_link_libraries(umicom-document-close-session-test PRIVATE Umicom::document)
umicom_apply_warnings(umicom-document-close-session-test)
umicom_apply_sanitizers(umicom-document-close-session-test)
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-document-close-session-test)
endif()
foreach(case IN ITEMS activation-guards format-status keep-missing missing-view readonly-save readonly-discard saved-before-turn
    invalid empty all-clean others keep-switch keep-closed only-kept
    cancel-before cancel-between cancel-question retry destroy-session destroy-owner
    separate-close reopened renamed-before repeated-step invalid-response stale
    saved-independently readonly-change closed-question external-conflict write-failure
    new-document other-active latest-before save discard untitled-save path-cancel
    path-stale bad-path format-counts format-capacity format-name format-phases)
    add_test(NAME framework.document_closing.sequence.${case}
        COMMAND umicom-document-close-session-test ${case})
    set_tests_properties(framework.document_closing.sequence.${case} PROPERTIES
        TIMEOUT 30 LABELS "framework;document;closing;sequence;regression")
endforeach()

add_executable(umicom-document-close-session-example ../../examples/editor_workflow/close_documents.c)
target_link_libraries(umicom-document-close-session-example PRIVATE Umicom::document)
umicom_apply_warnings(umicom-document-close-session-example)
umicom_apply_sanitizers(umicom-document-close-session-example)
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-document-close-session-example)
endif()
add_test(NAME framework.document_closing.sequence.example COMMAND umicom-document-close-session-example)
set_tests_properties(framework.document_closing.sequence.example PROPERTIES
    TIMEOUT 30 LABELS "framework;document;closing;sequence;example")
install(TARGETS umicom-document-close-session-example RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

if(TARGET umicom_ui_gtk4)
    add_executable(umicom-document-close-session-gtk4-test test_close_session_gtk4.c)
    target_link_libraries(umicom-document-close-session-gtk4-test PRIVATE Umicom::ui_gtk4 Umicom::document)
    umicom_apply_warnings(umicom-document-close-session-gtk4-test)
    umicom_apply_sanitizers(umicom-document-close-session-gtk4-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-document-close-session-gtk4-test)
    endif()
    # The link case calls real adapter APIs, but neither initialises GTK nor needs a display.
    add_test(NAME framework.document_closing.sequence.link.gtk4
        COMMAND umicom-document-close-session-gtk4-test link)
    set_tests_properties(framework.document_closing.sequence.link.gtk4 PROPERTIES
        TIMEOUT 30 LABELS "framework;document;closing;sequence;link")
    foreach(case empty clean others cancel-queued unbind-queued cancel-question cancel-command
        discard save stale unbind-question complete-unbind window-destroy save-busy single-busy)
        add_test(NAME framework.document_closing.sequence.gtk4.${case}
            COMMAND umicom-document-close-session-gtk4-test ${case})
        set_tests_properties(framework.document_closing.sequence.gtk4.${case} PROPERTIES
            TIMEOUT 30 SKIP_RETURN_CODE 77 RUN_SERIAL TRUE
            LABELS "framework;document;closing;sequence;gtk4;native")
    endforeach()
    add_custom_target(umicom-document-close-readiness)
    add_dependencies(umicom-document-close-readiness umicom-document-close-session-test
        umicom-document-close-session-example umicom-document-close-session-gtk4-test
        umicom-document-closing-gtk4-test)
else()
    add_custom_target(umicom-document-close-readiness
        COMMAND "${CMAKE_COMMAND}" -E echo "Document close readiness requires GTK4 and BUILD_TESTING=ON."
        COMMAND "${CMAKE_COMMAND}" -E false VERBATIM)
endif()
