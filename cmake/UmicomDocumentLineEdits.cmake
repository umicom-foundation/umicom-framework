# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
include_guard(GLOBAL)
if(BUILD_TESTING)
    add_executable(umicom-document-line-edit-test "${CMAKE_CURRENT_LIST_DIR}/../tests/document/test_line_edit.c")
    target_link_libraries(umicom-document-line-edit-test PRIVATE Umicom::document)
    umicom_apply_warnings(umicom-document-line-edit-test)
    umicom_apply_sanitizers(umicom-document-line-edit-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-document-line-edit-test)
    endif()
    foreach(case delete duplicate move-up move-down join indent outdent comment trim selected-indent selected-comment caret-only crlf unicode empty pending-typing read-only invalid-kind invalid-token no-neighbor no-change bare-cr closed null-owner undo-redo selection-history other-tab identity-other-tab identity-closed identity-zero identity-null-owner identity-read-only identity-history)
        add_test(NAME framework.document.line_edit.${case} COMMAND umicom-document-line-edit-test ${case})
        set_tests_properties(framework.document.line_edit.${case} PROPERTIES TIMEOUT 30 LABELS "framework;document;line-edit;history;regression")
    endforeach()
endif()

# Admission is shared by headless command clients and native menu controls.
if(BUILD_TESTING AND NOT TARGET umicom-document-line-admission-test)
    add_executable(umicom-document-line-admission-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/document/test_line_edit_admission.c")
    target_link_libraries(umicom-document-line-admission-test PRIVATE Umicom::document)
    umicom_apply_warnings(umicom-document-line-admission-test)
    umicom_apply_sanitizers(umicom-document-line-admission-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-document-line-admission-test)
    endif()
    foreach(case caret-kinds empty-selection selected-kinds read-only
            selection-transition readonly-transition zero-id null-owner unknown-id
            unsupported-kind closed other-tab history neighbor-deferred)
        add_test(NAME framework.document.line_admission.${case}
            COMMAND umicom-document-line-admission-test ${case})
        set_tests_properties(framework.document.line_admission.${case} PROPERTIES
            TIMEOUT 30 LABELS "framework;document;line-edit;availability;regression")
    endforeach()
endif()
