#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/development_workflow/CMakeLists.txt
# PURPOSE: Compile and run production development workflow contracts.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
add_executable(umicom-notes-debug-target notes_debuggee.c)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(umicom-notes-debug-target PRIVATE -g -O0)
endif()
add_executable(umicom-development-debug-test test_native_debug.c)
target_link_libraries(umicom-development-debug-test PRIVATE Umicom::Framework)
umicom_apply_warnings(umicom-development-debug-test)
umicom_apply_sanitizers(umicom-development-debug-test)
find_program(UMICOM_TEST_LLDB_DAP NAMES lldb-dap lldb-vscode)
find_program(UMICOM_TEST_GDB_DAP NAMES gdb)
foreach(kind lldb gdb)
    if(kind STREQUAL "lldb")
        set(_native_tool "${UMICOM_TEST_LLDB_DAP}")
        set(_native_probe_args --version)
    else()
        set(_native_tool "${UMICOM_TEST_GDB_DAP}")
        set(_native_probe_args --interpreter=dap --version)
    endif()
    if(_native_tool)
        execute_process(COMMAND "${_native_tool}" ${_native_probe_args}
            RESULT_VARIABLE _native_probe OUTPUT_QUIET ERROR_QUIET TIMEOUT 5)
    else()
        set(_native_probe 1)
    endif()
    if(NOT _native_probe EQUAL 0)
        set(_native_tool "unavailable-runtime-dependency")
    endif()
    add_test(NAME framework.development_workflow.native_debug.${kind} COMMAND umicom-development-debug-test
        "${_native_tool}" "$<TARGET_FILE:umicom-notes-debug-target>"
        "${CMAKE_CURRENT_SOURCE_DIR}/notes_debuggee.c" "13" "${kind}")
    set_tests_properties(framework.development_workflow.native_debug.${kind} PROPERTIES TIMEOUT 45 RUN_SERIAL TRUE SKIP_RETURN_CODE 77
        LABELS "framework;debug;external-debugger;regression")
endforeach()

# The deterministic peer exercises a real child process but does not claim to be
# GDB/LLDB. Its controlled responses make timing/order failures reproducible.
add_executable(umicom-dap-protocol-fixture dap_fixture.c)
target_link_libraries(umicom-dap-protocol-fixture PRIVATE Umicom::Framework)
umicom_apply_warnings(umicom-dap-protocol-fixture)
umicom_apply_sanitizers(umicom-dap-protocol-fixture)
add_executable(umicom-development-debug-session-test test_debug_session.c)
target_link_libraries(umicom-development-debug-session-test PRIVATE Umicom::Framework)
umicom_apply_warnings(umicom-development-debug-session-test)
umicom_apply_sanitizers(umicom-development-debug-session-test)
foreach(case launch-late launch-early initialized-first reject-launch no-initialized
    initial-breakpoints disabled-breakpoints frame-zero inspect-stopped watch-frame-zero
    step-over step-into step-out continue-exit restart invalid-launch)
    add_test(NAME framework.development_workflow.debug.${case} COMMAND umicom-development-debug-session-test
        "$<TARGET_FILE:umicom-dap-protocol-fixture>" "${case}")
    set_tests_properties(framework.development_workflow.debug.${case} PROPERTIES TIMEOUT 20 RUN_SERIAL TRUE
        LABELS "framework;debug;protocol-fixture;regression")
endforeach()

add_executable(umicom-development-workflow-example ../../examples/development_workflow/main.c)
target_link_libraries(umicom-development-workflow-example PRIVATE Umicom::Framework)
umicom_apply_warnings(umicom-development-workflow-example)
umicom_apply_sanitizers(umicom-development-workflow-example)
foreach(case complete compile-failure test-failure no-tests no-cpack deploy-test-failure)
    add_test(NAME framework.development_workflow.delivery.${case}
        COMMAND umicom-development-workflow-example "${case}")
    set_tests_properties(framework.development_workflow.delivery.${case} PROPERTIES
        TIMEOUT 180 RUN_SERIAL TRUE LABELS "framework;build;package;real-toolchain;regression")
endforeach()

add_executable(umicom-development-delivery-contract-test test_delivery_contracts.c)
target_link_libraries(umicom-development-delivery-contract-test PRIVATE Umicom::build)
umicom_apply_warnings(umicom-development-delivery-contract-test)
umicom_apply_sanitizers(umicom-development-delivery-contract-test)
foreach(case provider-argv provider-invalid tests-disabled package-plan deploy-plan rebuild-plan cancel test-refusal)
    add_test(NAME framework.development_workflow.contract.${case}
        COMMAND umicom-development-delivery-contract-test "${case}")
    set_tests_properties(framework.development_workflow.contract.${case} PROPERTIES TIMEOUT 20
        LABELS "framework;build;contract;regression")
endforeach()

foreach(target umicom-development-debug-test umicom-development-debug-session-test
    umicom-development-workflow-example umicom-development-delivery-contract-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(${target})
    endif()
endforeach()
add_dependencies(umicom-development-debug-test umicom-notes-debug-target)
add_dependencies(umicom-development-debug-session-test umicom-dap-protocol-fixture)
install(TARGETS umicom-development-workflow-example RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../../examples/development_workflow/"
    DESTINATION "${CMAKE_INSTALL_DATADIR}/umicom/examples/development_workflow")
