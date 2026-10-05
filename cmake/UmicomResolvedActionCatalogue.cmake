# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
include_guard(GLOBAL)
if(BUILD_TESTING)
    add_executable(umicom-resolved-action-catalogue-test "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_resolved_action_catalogue.c")
    target_link_libraries(umicom-resolved-action-catalogue-test PRIVATE Umicom::developer)
    umicom_apply_warnings(umicom-resolved-action-catalogue-test)
    umicom_apply_sanitizers(umicom-resolved-action-catalogue-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-resolved-action-catalogue-test)
    endif()
    foreach(case valid reordered owned-input no-data empty-edit changed-title changed-kind changed-data changed-preferred changed-diagnostics changed-extension missing-title missing-data missing-extension missing-edit added-command added-data edit-null edit-array result-null result-array disabled command existing-edit escaped-text numeric-spelling duplicate-data nested-duplicate index null-catalogue null-json null-output wrong-id error cancelled oversized field-limit)
        add_test(NAME framework.language_runtime.resolved_action_catalogue.${case} COMMAND umicom-resolved-action-catalogue-test ${case})
        set_tests_properties(framework.language_runtime.resolved_action_catalogue.${case} PROPERTIES TIMEOUT 30
            LABELS "framework;language;locations;ownership;regression")
    endforeach()
endif()

install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/action-resolution.html" "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/json-value-comparison.html" DESTINATION "${CMAKE_INSTALL_DATADIR}/doc/umicom/learning" COMPONENT Development)
