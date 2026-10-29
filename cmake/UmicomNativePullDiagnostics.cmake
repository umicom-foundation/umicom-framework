# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
include_guard(GLOBAL)
if(BUILD_TESTING)
    add_executable(umicom-native-pull-diagnostics-test "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/test_native_pull_diagnostics.c")
    target_link_libraries(umicom-native-pull-diagnostics-test PRIVATE Umicom::developer Umicom::editor Umicom::platform)
    umicom_apply_warnings(umicom-native-pull-diagnostics-test)
    umicom_apply_sanitizers(umicom-native-pull-diagnostics-test)
    if(WIN32 AND CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_link_options(umicom-native-pull-diagnostics-test PRIVATE -municode)
    endif()
    add_dependencies(umicom-native-pull-diagnostics-test umicom-language-process-fixture)
    # Failure-report headers affect qualification even though they are not translation units.
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umicom-native-pull-diagnostics-test FILES
            "${CMAKE_CURRENT_LIST_DIR}/../tests/language_runtime/query_failure_report.h")
    endif()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-native-pull-diagnostics-test)
    endif()
    foreach(case valid unicode empty literal invalid timeout encoding shutdown cancel surrogate outside related choices unchanged related-report unsupported query-error)
        add_test(NAME framework.language_runtime.native_pull_diagnostics.${case} COMMAND umicom-native-pull-diagnostics-test ${case} "$<TARGET_FILE:umicom-language-process-fixture>")
        set_tests_properties(framework.language_runtime.native_pull_diagnostics.${case} PROPERTIES TIMEOUT 40
            LABELS "framework;language;diagnostics;native;ownership;regression")
        # The independent peer is a runtime dependency, not a linked library.
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(framework.language_runtime.native_pull_diagnostics.${case}
                TARGETS umicom-native-pull-diagnostics-test umicom-language-process-fixture)
        endif()
    endforeach()
endif()
