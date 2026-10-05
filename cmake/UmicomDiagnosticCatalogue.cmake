# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
include_guard(GLOBAL)
if(BUILD_TESTING)
    add_executable(umicom-diagnostic-catalogue-test "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_diagnostic_catalogue.c")
    target_link_libraries(umicom-diagnostic-catalogue-test PRIVATE Umicom::developer)
    umicom_apply_warnings(umicom-diagnostic-catalogue-test)
    umicom_apply_sanitizers(umicom-diagnostic-catalogue-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-diagnostic-catalogue-test)
    endif()
    foreach(case basic unversioned zero-version negative-version null-version large-version error warning information hint invalid-severity severity-type numeric-code string-code empty-code code-null code-large source source-type description description-missing description-uri tags tags-type tag-zero tag-type data data-null unicode literal empty-message message-type related related-empty related-type related-bad related-message-type reversed-range negative-position range-type missing-message late-invalid empty null array missing-uri missing-diagnostics uri-type uri-escape diagnostics-type duplicate-uri duplicate-message owned notification wrong-method notification-id notification-error notification-protocol notification-params notification-duplicate cancelled arguments count-limit count-boundary related-limit related-total-limit message-limit source-limit code-limit tag-limit validate validate-uri validate-version validate-unknown-version validate-unversioned validate-outside validate-surrogate validate-unicode validate-crlf validate-invalid-source validate-related validate-cancelled raw)
        add_test(NAME framework.language_runtime.diagnostic_catalogue.${case} COMMAND umicom-diagnostic-catalogue-test ${case})
        set_tests_properties(framework.language_runtime.diagnostic_catalogue.${case} PROPERTIES TIMEOUT 30
            LABELS "framework;language;diagnostics;ownership;regression")
    endforeach()
endif()

install(FILES "${CMAKE_CURRENT_LIST_DIR}/../docs/learning/source-diagnostics.html" DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/docs/learning")
