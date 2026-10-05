# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
include_guard(GLOBAL)
if(BUILD_TESTING)
    add_executable(umicom-workspace-symbol-query-test "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_workspace_symbol_query.c")
    target_link_libraries(umicom-workspace-symbol-query-test PRIVATE Umicom::developer Umicom::editor Umicom::platform)
    umicom_apply_warnings(umicom-workspace-symbol-query-test)
    umicom_apply_sanitizers(umicom-workspace-symbol-query-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-workspace-symbol-query-test)
    endif()
    foreach(case valid object empty empty-query escaped-query unicode-query query-null query-invalid query-capacity query-escape-capacity external self-outside self-surrogate missing disabled document-only encoding sync fragmented notification error hierarchy mixed missing-range invalid-result shutdown-error close-error timeout cancel-before cancel-during invalid-source state read-error write-error wrong-id)
        add_test(NAME framework.language_runtime.workspace_symbol_query.${case} COMMAND umicom-workspace-symbol-query-test ${case})
        set_tests_properties(framework.language_runtime.workspace_symbol_query.${case} PROPERTIES TIMEOUT 30
            LABELS "framework;language;navigation;ownership;regression")
    endforeach()
endif()

install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/workspace-symbols.html" DESTINATION "${CMAKE_INSTALL_DATADIR}/doc/umicom/learning" COMPONENT Development)
