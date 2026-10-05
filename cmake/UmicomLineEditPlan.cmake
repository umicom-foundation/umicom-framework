# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
include_guard(GLOBAL)
if(BUILD_TESTING)
    add_executable(umicom-line-edit-plan-test "${CMAKE_CURRENT_LIST_DIR}/../tests/editor/test_line_edit_plan.c")
    target_link_libraries(umicom-line-edit-plan-test PRIVATE Umicom::editor Umicom::platform)
    umicom_apply_warnings(umicom-line-edit-plan-test)
    umicom_apply_sanitizers(umicom-line-edit-plan-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-line-edit-plan-test)
    endif()
    foreach(case delete-middle delete-last delete-only duplicate-middle duplicate-last duplicate-crlf duplicate-empty move-up move-down move-unequal move-unicode move-top move-bottom join join-empty join-crlf-empty indent indent-selection indent-reverse indent-tab outdent outdent-tab comment uncomment comment-mixed comment-token comment-blank trim trim-unchanged owned invalid-utf8 split-scalar split-crlf bare-cr embedded-null invalid-kind invalid-indent invalid-comment oversized-indent oversized-comment null-token invalid-cursor invalid-selection cancelled null-output capacity)
        add_test(NAME framework.editor.line_edit_plan.${case} COMMAND umicom-line-edit-plan-test ${case})
        set_tests_properties(framework.editor.line_edit_plan.${case} PROPERTIES TIMEOUT 30 LABELS "framework;editor;line-edit;ownership;regression")
    endforeach()
endif()

include("${CMAKE_CURRENT_LIST_DIR}/UmicomDocumentLineEdits.cmake")

include("${CMAKE_CURRENT_LIST_DIR}/UmicomNativeLineEdits.cmake")

include("${CMAKE_CURRENT_LIST_DIR}/UmicomLineEditCommands.cmake")

install(FILES "${CMAKE_CURRENT_LIST_DIR}/../learning/line-editing.html" DESTINATION ${CMAKE_INSTALL_DATADIR}/doc/umicom/learning COMPONENT Development)

include("${CMAKE_CURRENT_LIST_DIR}/UmicomDocumentFormat.cmake")
