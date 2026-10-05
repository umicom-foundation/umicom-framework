# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
include_guard(GLOBAL)
if(BUILD_TESTING)
    # CMakeLists.txt owns umicom-diagnostic-query-test for the core diagnostics
    # test in tests/test_diagnostic_query.c. The language-runtime suite below
    # is a different executable and must have its own globally unique target.
    # Retain the previous registration, disabled, for engineering review. Its
    # replacement below preserves the source, CTest names, case arguments,
    # linked libraries, warnings, sanitizers, validation membership and labels.
    if(FALSE)
        add_executable(umicom-diagnostic-query-test "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_diagnostic_query.c")
        target_link_libraries(umicom-diagnostic-query-test PRIVATE Umicom::developer Umicom::editor Umicom::platform)
        umicom_apply_warnings(umicom-diagnostic-query-test)
        umicom_apply_sanitizers(umicom-diagnostic-query-test)
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(umicom-diagnostic-query-test)
        endif()
        foreach(case valid object encoding sync fragmented noise wrong-uri stale-version unversioned empty invalid-content outside surrogate related shutdown-error close-error timeout publication-timeout cancel-before cancel-during invalid-source state native-invalid read-error write-error escaped-limit server-request unknown-tag future-version)
            add_test(NAME framework.language_runtime.diagnostic_query.${case} COMMAND umicom-diagnostic-query-test ${case})
            set_tests_properties(framework.language_runtime.diagnostic_query.${case} PROPERTIES TIMEOUT 30
                LABELS "framework;language;diagnostic;ownership;regression")
        endforeach()
    endif()

    add_executable(umicom-language-runtime-diagnostic-query-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_diagnostic_query.c")
    target_link_libraries(umicom-language-runtime-diagnostic-query-test PRIVATE
        Umicom::developer
        Umicom::editor
        Umicom::platform)
    umicom_apply_warnings(umicom-language-runtime-diagnostic-query-test)
    umicom_apply_sanitizers(umicom-language-runtime-diagnostic-query-test)

    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(
            umicom-language-runtime-diagnostic-query-test)
    endif()

    foreach(case
        valid object encoding sync fragmented noise
        wrong-uri stale-version unversioned empty invalid-content outside
        surrogate related shutdown-error close-error timeout publication-timeout
        cancel-before cancel-during invalid-source state native-invalid
        read-error write-error escaped-limit server-request unknown-tag
        future-version)
        add_test(
            NAME framework.language_runtime.diagnostic_query.${case}
            COMMAND umicom-language-runtime-diagnostic-query-test ${case})
        set_tests_properties(
            framework.language_runtime.diagnostic_query.${case}
            PROPERTIES
                TIMEOUT 30
                LABELS "framework;language;diagnostic;ownership;regression")
    endforeach()
endif()
