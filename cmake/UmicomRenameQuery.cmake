# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
include_guard(GLOBAL)
if(BUILD_TESTING)
    add_executable(umicom-rename-query-test "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_rename_query.c")
    target_link_libraries(umicom-rename-query-test PRIVATE Umicom::developer Umicom::editor Umicom::platform)
    umicom_apply_warnings(umicom-rename-query-test)
    umicom_apply_sanitizers(umicom-rename-query-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-rename-query-test)
    endif()
    foreach(case valid object missing disabled encoding sync malformed-prepare-option prepare-disabled fragmented notification error shutdown-error close-error timeout cancel-before cancel-during invalid-source invalid-caret state unicode native-invalid read-error write-error escaped-limit name-null name-empty name-control name-utf8 name-capacity name-unicode name-escaped wrong-id prepare-error prepare-range prepare-placeholder prepare-null prepare-default prepare-outside prepare-reversed prepare-away prepare-end prepare-empty-range prepare-missing-placeholder prepare-invalid-placeholder prepare-stray-placeholder prepare-invalid-point prepare-surrogate prepare-unicode prepare-wrong-id empty versioned unversioned stale-version annotations overlap invalid-content result-range-outside resource-operation external-document multiple-documents missing-annotation)
        add_test(NAME framework.language_runtime.rename_query.${case} COMMAND umicom-rename-query-test ${case})
        set_tests_properties(framework.language_runtime.rename_query.${case} PROPERTIES TIMEOUT 30
            LABELS "framework;language;rename;ownership;regression")
    endforeach()
endif()

install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/symbol-rename.html" DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/docs/learning")
