# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
# Register the same process fixtures in the full build and the focused adapter project.
include_guard(GLOBAL)
add_executable(umicom-live-test-output-example "${CMAKE_CURRENT_LIST_DIR}/../../examples/testing/live_output.c")
add_executable(umicom-live-test-output-child "${CMAKE_CURRENT_LIST_DIR}/child.c")
add_executable(umicom-live-test-output-tail-check "${CMAKE_CURRENT_LIST_DIR}/test_tail.c")
add_executable(umicom-live-test-output-execution-check "${CMAKE_CURRENT_LIST_DIR}/test_execution.c")
if(TARGET Umicom::testing)
    target_link_libraries(umicom-live-test-output-example PRIVATE Umicom::testing)
    target_link_libraries(umicom-live-test-output-tail-check PRIVATE Umicom::platform)
    target_link_libraries(umicom-live-test-output-execution-check PRIVATE Umicom::testing)
else()
    target_link_libraries(umicom-live-test-output-example PRIVATE umicom-ctest-checks-core umicom-ctest-checks-process)
    target_link_libraries(umicom-live-test-output-tail-check PRIVATE umicom-ctest-checks-core)
    target_link_libraries(umicom-live-test-output-execution-check PRIVATE
        umicom-ctest-checks-core umicom-ctest-checks-process)
endif()
add_dependencies(umicom-live-test-output-execution-check umicom-live-test-output-child)
foreach(target umicom-live-test-output-example umicom-live-test-output-child umicom-live-test-output-tail-check umicom-live-test-output-execution-check)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(${target})
        umicom_apply_sanitizers(${target})
    endif()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(${target})
    endif()
endforeach()
foreach(case empty exact rolling oversized binary saturated alias invalid)
    add_test(NAME "framework.output_tail.${case}" COMMAND umicom-live-test-output-tail-check "${case}")
    set_tests_properties("framework.output_tail.${case}" PROPERTIES TIMEOUT 30 LABELS "framework;platform;output;regression")
endforeach()
set(output_fixture "${CMAKE_CURRENT_BINARY_DIR}/qualification/live-test-output/$<CONFIG>")
add_test(NAME framework.live_test_output.setup COMMAND "${CMAKE_COMMAND}"
    -S "${CMAKE_CURRENT_LIST_DIR}/fixture" -B "${output_fixture}"
    "-DHELPER=$<TARGET_FILE:umicom-live-test-output-child>")
set_tests_properties(framework.live_test_output.setup PROPERTIES
    TIMEOUT 60 FIXTURES_SETUP umicom_live_test_output LABELS "framework;testing;fixture")
foreach(case invalid created live cancel timeout repeat next-attempt large disabled failed skipped missing
        unicode observer aliased legacy pre-cancel)
    add_test(NAME "framework.live_test_output.execution.${case}" COMMAND umicom-live-test-output-execution-check
        "${case}" "${output_fixture}")
    set_tests_properties("framework.live_test_output.execution.${case}" PROPERTIES TIMEOUT 30
        FIXTURES_REQUIRED umicom_live_test_output RESOURCE_LOCK umicom_live_test_output
        LABELS "framework;testing;output;process-fixture;regression")
endforeach()
