# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
include_guard(GLOBAL)
# Persistence uses the existing developer JSON owner; the parser-only editor target remains independent.
target_sources(umicom_developer PRIVATE "${CMAKE_CURRENT_LIST_DIR}/../src/developer_project/snippet_storage.c")

if(BUILD_TESTING)
    add_executable(umicom-snippet-storage-test "${CMAKE_CURRENT_LIST_DIR}/../tests/editor/test_snippet_storage.c")
    target_link_libraries(umicom-snippet-storage-test PRIVATE Umicom::developer)
    umicom_apply_warnings(umicom-snippet-storage-test)
    umicom_apply_sanitizers(umicom-snippet-storage-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-snippet-storage-test)
    endif()
    foreach(case round-trip unicode escaped empty-body body-limit encode-capacity unterminated invalid-unicode structure language-empty bytes-limit output-arguments path-case path-language path-separator path-capacity path-id-limit path-language-limit path-invalid-base save-load replace invalid-save wrong-id wrong-language missing-file json-array json-null missing unknown duplicate wrong-type wrong-format empty-id blank-name control-id nul-body bad-surrogate trailing)
        add_test(NAME framework.editor.snippet_storage.${case} COMMAND umicom-snippet-storage-test ${case})
        set_tests_properties(framework.editor.snippet_storage.${case} PROPERTIES TIMEOUT 30 LABELS "framework;editor;snippet;storage;regression")
    endforeach()
endif()
