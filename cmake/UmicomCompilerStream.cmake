# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Exercise both arbitrary byte segmentation and a real long-lived output pipe.
include_guard(GLOBAL)
if(BUILD_TESTING)
    add_executable(umicom-compiler-stream-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/compiler_stream/test_stream.c")
    add_executable(umicom-compiler-stream-runner-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/compiler_stream/test_runner.c")
    add_executable(umicom-compiler-stream-output-producer
        "${CMAKE_CURRENT_LIST_DIR}/../tests/compiler_stream/output_producer.c")
    add_executable(umicom-build-diagnostic-progress-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/compiler_stream/test_progress.c")
    foreach(target umicom-compiler-stream-test umicom-compiler-stream-runner-test
                   umicom-build-diagnostic-progress-test)
        target_link_libraries(${target} PRIVATE Umicom::build)
    endforeach()
    foreach(target umicom-compiler-stream-test umicom-compiler-stream-runner-test
                   umicom-compiler-stream-output-producer umicom-build-diagnostic-progress-test)
        set_target_properties(${target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES)
        umicom_apply_warnings(${target})
        umicom_apply_sanitizers(${target})
        if(COMMAND umicom_register_validation_target)
            umicom_register_validation_target(${target})
        endif()
    endforeach()
    foreach(case split bytes lifecycle nul long-line long-continuation record-bound)
        add_test(NAME framework.compiler_stream.${case}
            COMMAND umicom-compiler-stream-test "${case}")
        set_tests_properties(framework.compiler_stream.${case} PROPERTIES
            TIMEOUT 30 LABELS "framework;build;diagnostics;stream;regression")
    endforeach()
    add_test(NAME framework.compiler_stream.native-output
        COMMAND umicom-compiler-stream-runner-test
            "$<TARGET_FILE:umicom-compiler-stream-output-producer>"
            "${CMAKE_CURRENT_BINARY_DIR}")
    set_tests_properties(framework.compiler_stream.native-output PROPERTIES
        TIMEOUT 30 LABELS "framework;build;diagnostics;native;regression")
    foreach(case early many damaged phases cancel failure legacy invalid next-run)
        add_test(NAME framework.build_diagnostic_progress.${case}
            COMMAND umicom-build-diagnostic-progress-test "${case}")
        set_tests_properties(framework.build_diagnostic_progress.${case} PROPERTIES
            TIMEOUT 30 LABELS "framework;build;diagnostics;worker;regression")
    endforeach()
endif()


# Page reads exercise the producer/reader boundary before a phase finishes.
if(BUILD_TESTING)
    add_executable(umicom-build-diagnostic-pages-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/compiler_stream/test_pages.c")
    target_link_libraries(umicom-build-diagnostic-pages-test PRIVATE Umicom::build)
    umicom_apply_warnings(umicom-build-diagnostic-pages-test)
    umicom_apply_sanitizers(umicom-build-diagnostic-pages-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-build-diagnostic-pages-test)
    endif()
    foreach(case pages capacity damaged invalid phases cancel repeat projection)
        add_test(NAME framework.build_diagnostic_pages.${case}
            COMMAND umicom-build-diagnostic-pages-test "${case}")
        set_tests_properties(framework.build_diagnostic_pages.${case} PROPERTIES
            TIMEOUT 30 LABELS "framework;build;diagnostics;worker;regression")
    endforeach()
endif()
