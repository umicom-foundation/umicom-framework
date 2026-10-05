# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
include_guard(GLOBAL)
if(BUILD_TESTING)
    add_executable(umicom-diagnostic-context-test "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_diagnostic_context.c")
    target_link_libraries(umicom-diagnostic-context-test PRIVATE Umicom::developer)
    umicom_apply_warnings(umicom-diagnostic-context-test)
    umicom_apply_sanitizers(umicom-diagnostic-context-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-diagnostic-context-test)
    endif()
    foreach(case selection caret-start caret-middle caret-end whole empty-set owned wrong-uri wrong-version unknown-version unversioned reversed outside split-utf8 invalid-source invalid-diagnostic cancelled arguments raw)
        add_test(NAME framework.language_runtime.diagnostic_context.${case} COMMAND umicom-diagnostic-context-test ${case})
        set_tests_properties(framework.language_runtime.diagnostic_context.${case} PROPERTIES TIMEOUT 30
            LABELS "framework;language;diagnostics;ownership;regression")
    endforeach()
endif()

install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/diagnostic-backed-actions.html" DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/docs/learning")
