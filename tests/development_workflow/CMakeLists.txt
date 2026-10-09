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

# Launch contracts compare the production build and debugger argument parsers.
add_executable(umicom-launch-arguments-test "${CMAKE_CURRENT_LIST_DIR}/test_launch_arguments.c")
target_link_libraries(umicom-launch-arguments-test PRIVATE Umicom::build Umicom::developer)
umicom_apply_warnings(umicom-launch-arguments-test)
umicom_apply_sanitizers(umicom-launch-arguments-test)
foreach(case boundaries literal reversible invalid)
    add_test(NAME framework.development_workflow.arguments.${case}
        COMMAND umicom-launch-arguments-test ${case})
    set_tests_properties(framework.development_workflow.arguments.${case} PROPERTIES
        TIMEOUT 20 LABELS "framework;build;debug;arguments;regression")
endforeach()

# This scenario uses the controlled peer already shared by debugger lifecycle
# checks. It verifies native launch serialization without a broker or debuggee.
add_executable(umicom-argument-launch-test "${CMAKE_CURRENT_LIST_DIR}/test_argument_launch.c")
target_link_libraries(umicom-argument-launch-test PRIVATE Umicom::Framework)
umicom_apply_warnings(umicom-argument-launch-test)
umicom_apply_sanitizers(umicom-argument-launch-test)
add_test(NAME framework.development_workflow.arguments.native-launch
    COMMAND umicom-argument-launch-test "$<TARGET_FILE:umicom-dap-protocol-fixture>")
set_tests_properties(framework.development_workflow.arguments.native-launch PROPERTIES
    TIMEOUT 20 RUN_SERIAL TRUE LABELS "framework;debug;arguments;protocol-fixture;regression")

# Building this focused target must also produce the peer it launches. Register
# both checks with the existing qualification target when the host provides it.
add_dependencies(umicom-argument-launch-test umicom-dap-protocol-fixture)
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-launch-arguments-test)
    umicom_register_validation_target(umicom-argument-launch-test)
endif()

# Restart qualification uses the existing controlled peer and production runtime.
add_executable(umicom-debug-restart-session-test ../debug_restart/test_session.c)
target_link_libraries(umicom-debug-restart-session-test PRIVATE Umicom::Framework)
umicom_apply_warnings(umicom-debug-restart-session-test)
umicom_apply_sanitizers(umicom-debug-restart-session-test)
add_dependencies(umicom-debug-restart-session-test umicom-dap-protocol-fixture)
foreach(case supported unsupported rejected timeout wrong-command event-first zero queued
    reconfigure-before reconfigure-after reconfigure-rejected reconfigure-timeout)
    add_test(NAME framework.debug_restart.${case} COMMAND umicom-debug-restart-session-test
        "$<TARGET_FILE:umicom-dap-protocol-fixture>" "${case}")
    set_tests_properties(framework.debug_restart.${case} PROPERTIES
        TIMEOUT 20 LABELS "framework;debugger;restart;protocol-fixture;regression")
endforeach()
add_executable(umicom-debug-watch-invalidation-test ../debug_restart/test_watches.c)
target_link_libraries(umicom-debug-watch-invalidation-test PRIVATE Umicom::Framework)
umicom_apply_warnings(umicom-debug-watch-invalidation-test)
umicom_apply_sanitizers(umicom-debug-watch-invalidation-test)
add_test(NAME framework.debug_restart.watch_choices COMMAND umicom-debug-watch-invalidation-test)
set_tests_properties(framework.debug_restart.watch_choices PROPERTIES TIMEOUT 20
    LABELS "framework;debugger;restart;regression")
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-debug-restart-session-test)
    umicom_register_validation_target(umicom-debug-watch-invalidation-test)
endif()

# Isolated adapters have the builtin filenames, so the selected-folder path is
# exercised without depending on a developer's installed GDB or LLDB.
add_executable(umicom-tool-debugger-test ../tool_location/test_debugger.c)
target_link_libraries(umicom-tool-debugger-test PRIVATE Umicom::Framework)
umicom_apply_warnings(umicom-tool-debugger-test)
umicom_apply_sanitizers(umicom-tool-debugger-test)
if(WIN32 AND CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_link_options(umicom-tool-debugger-test PRIVATE -municode)
endif()
foreach(kind lldb gdb)
    add_executable(umicom-tool-${kind}-peer dap_fixture.c)
    target_link_libraries(umicom-tool-${kind}-peer PRIVATE Umicom::Framework)
    umicom_apply_warnings(umicom-tool-${kind}-peer)
    umicom_apply_sanitizers(umicom-tool-${kind}-peer)
    set_target_properties(umicom-tool-${kind}-peer PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/isolated debugger tools")
    if(kind STREQUAL "lldb")
        set_target_properties(umicom-tool-${kind}-peer PROPERTIES OUTPUT_NAME lldb-dap)
    else()
        set_target_properties(umicom-tool-${kind}-peer PROPERTIES OUTPUT_NAME gdb)
    endif()
    add_dependencies(umicom-tool-debugger-test umicom-tool-${kind}-peer)
endforeach()
foreach(case lldb gdb absolute invalid-folder relative-adapter missing)
    add_test(NAME framework.tool_location.debugger.${case}
        COMMAND umicom-tool-debugger-test "${case}" "$<TARGET_FILE_DIR:umicom-tool-lldb-peer>"
            "$<TARGET_FILE:umicom-tool-lldb-peer>")
    set_tests_properties(framework.tool_location.debugger.${case} PROPERTIES
        TIMEOUT 30 LABELS "framework;debugger;tool-location;protocol-fixture;regression")
endforeach()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-tool-debugger-test)
endif()
