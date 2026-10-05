# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
include_guard(GLOBAL)
# Indexed snapshots use the shared atomic cancellation token. Static consumers
# receive this dependency through the editor target's link interface.
target_link_libraries(umicom_editor PRIVATE Umicom::platform)
if(BUILD_TESTING)
    add_executable(umicom-text-position-index-test "${CMAKE_CURRENT_LIST_DIR}/../tests/editor/test_text_position_index.c")
    target_link_libraries(umicom-text-position-index-test PRIVATE Umicom::editor)
    umicom_apply_warnings(umicom-text-position-index-test)
    umicom_apply_sanitizers(umicom-text-position-index-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-text-position-index-test)
    endif()
    foreach(case empty ascii end lf crlf cr trailing-lf trailing-cr trailing-crlf mixed empty-line accent combining supplementary cjk invalid-leading invalid-continuation overlong encoded-surrogate out-of-unicode truncated invalid-prefix ownership many-lines checkpoint-unicode checkpoint-crlf embedded-nul empty-null limit inside-utf8 inside-surrogate inside-crlf invalid-suffix cancelled past-line past-column past-byte invalid-view arguments)
        add_test(NAME framework.editor.text_position_index.${case} COMMAND umicom-text-position-index-test ${case})
        set_tests_properties(framework.editor.text_position_index.${case} PROPERTIES TIMEOUT 30
            LABELS "framework;editor;unicode;ownership;regression")
    endforeach()
endif()
