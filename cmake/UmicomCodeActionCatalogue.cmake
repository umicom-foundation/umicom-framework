# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
include_guard(GLOBAL)
if(BUILD_TESTING)
    add_executable(umicom-code-action-catalogue-test "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_code_action_catalogue.c")
    target_link_libraries(umicom-code-action-catalogue-test PRIVATE Umicom::developer)
    umicom_apply_warnings(umicom-code-action-catalogue-test)
    umicom_apply_sanitizers(umicom-code-action-catalogue-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-code-action-catalogue-test)
    endif()
    foreach(case basic null empty command nested-command edit-and-command disabled disabled-empty-reason deferred title-only unknown-kind empty-kind unicode literal-title opaque-fields resource invalid-edit annotation late-invalid root-object item-null missing-title empty-title kind-type preferred-type disabled-type disabled-missing-reason reason-type edit-type diagnostics-type command-null command-missing-title command-empty arguments-type mixed-command-edit mixed-command-disabled duplicate-title duplicate-edit duplicate-command invalid-utf8 invalid-surrogate owned response wrong-id error cancelled arguments count-limit count-boundary title-limit kind-limit reason-limit command-limit selected-good preview)
        add_test(NAME framework.language_runtime.code_action_catalogue.${case} COMMAND umicom-code-action-catalogue-test ${case})
        set_tests_properties(framework.language_runtime.code_action_catalogue.${case} PROPERTIES TIMEOUT 30
            LABELS "framework;language;code-actions;ownership;regression")
    endforeach()
endif()

install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/code-actions.html" DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/docs/learning")
