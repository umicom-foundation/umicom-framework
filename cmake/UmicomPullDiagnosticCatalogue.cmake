# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
include_guard(GLOBAL)
if(BUILD_TESTING)
    add_executable(umicom-pull-diagnostic-catalogue-test "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_pull_diagnostic_catalogue.c")
    target_link_libraries(umicom-pull-diagnostic-catalogue-test PRIVATE Umicom::developer)
    umicom_apply_warnings(umicom-pull-diagnostic-catalogue-test)
    umicom_apply_sanitizers(umicom-pull-diagnostic-catalogue-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-pull-diagnostic-catalogue-test)
    endif()
    foreach(case full empty result-id no-result-id empty-id unicode-id id-type duplicate-id unchanged unknown-kind missing-kind kind-type missing-items items-type invalid-row related-empty related-present related-type duplicate-items duplicate-kind wrong-id remote-error null-result protocol method duplicate-result owned raw-data uri-invalid uri-null cancel arguments)
        add_test(NAME framework.language_runtime.pull_diagnostic_catalogue.${case} COMMAND umicom-pull-diagnostic-catalogue-test ${case})
        set_tests_properties(framework.language_runtime.pull_diagnostic_catalogue.${case} PROPERTIES TIMEOUT 30
            LABELS "framework;language;diagnostics;ownership;regression")
    endforeach()
endif()

install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/pull-diagnostics.html" DESTINATION ${CMAKE_INSTALL_DATADIR}/umicom-framework/docs/learning)
