#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomDebugRuntimePlatform.cmake
#
# PURPOSE:
#   Attach the operational Debug Adapter Protocol runtime to the canonical
#   developer library and register its focused regression tests.
#
# ARCHITECTURE:
#   Existing Debug Service, advanced inspection, memory/disassembly/register
#   views and stable adapter contracts remain authoritative. This module supplies
#   the missing persistent DAP transport/protocol and bridges real adapter data
#   into those established models.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)

# Load the dependency only when the parent build has not already provided its target.
if(NOT TARGET umicom_developer)
    message(FATAL_ERROR
        "Debug Runtime requires the canonical umicom_developer target")
endif()

target_sources(umicom_developer PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/adapter.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/base64.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/builtin_profiles.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/capabilities.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/command_router.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/contract_adapter.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/decoder_support.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/decoders/breakpoints.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/decoders/completions.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/decoders/data_breakpoint_info.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/decoders/disassembly.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/decoders/evaluate.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/decoders/event.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/decoders/exception_info.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/decoders/initialize.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/decoders/memory.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/decoders/modules.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/decoders/scopes.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/decoders/source.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/decoders/stack_trace.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/decoders/threads.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/decoders/variables.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/event_queue.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/message.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/pending.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/platform.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/profile_health.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/profiles/codelldb.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/profiles/cppvsdbg.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/profiles/debugpy.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/profiles/gdb_dap.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/profiles/java_debug.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/profiles/js_debug.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/profiles/lldb_dap.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/profiles/netcoredbg.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/profiles/php_debug.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/request_support.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/requests/attach.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/requests/breakpoint_locations.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/requests/cancel.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/requests/completions.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/requests/configuration_done.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/requests/continue.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/requests/data_breakpoint_info.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/requests/disassemble.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/requests/disconnect.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/requests/evaluate.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/requests/exception_info.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/requests/goto.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/requests/goto_targets.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/requests/initialize.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/requests/launch.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/requests/loaded_sources.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/requests/modules.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/requests/next.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/requests/pause.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/requests/read_memory.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/requests/restart.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/requests/restart_frame.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/requests/reverse_continue.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/requests/scopes.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/requests/set_breakpoints.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/requests/set_data_breakpoints.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/requests/set_exception_breakpoints.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/requests/set_expression.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/requests/set_function_breakpoints.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/requests/set_instruction_breakpoints.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/requests/set_variable.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/requests/source.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/requests/stack_trace.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/requests/step_back.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/requests/step_in.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/requests/step_in_targets.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/requests/step_out.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/requests/terminate.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/requests/terminate_threads.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/requests/threads.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/requests/variables.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/requests/write_memory.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/response_queue.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/service_bridge.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/transport.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/types.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/debug_runtime/workbench_bridge.c"
)

# Register verification targets only when the developer has enabled testing.
if(BUILD_TESTING)
    add_executable(
        umicom-debug-runtime-adapter-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_adapter.c"
    )
    target_include_directories(
        umicom-debug-runtime-adapter-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-adapter-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-adapter-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-adapter-test)
    endif()
    add_test(NAME framework.debug_runtime.adapter COMMAND umicom-debug-runtime-adapter-test)

    add_executable(
        umicom-debug-runtime-adapter-out-of-order-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_adapter_out_of_order.c"
    )
    target_include_directories(
        umicom-debug-runtime-adapter-out-of-order-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-adapter-out-of-order-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-adapter-out-of-order-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-adapter-out-of-order-test)
    endif()
    add_test(NAME framework.debug_runtime.adapter-out-of-order COMMAND umicom-debug-runtime-adapter-out-of-order-test)

    add_executable(
        umicom-debug-runtime-base64-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_base64.c"
    )
    target_include_directories(
        umicom-debug-runtime-base64-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-base64-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-base64-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-base64-test)
    endif()
    add_test(NAME framework.debug_runtime.base64 COMMAND umicom-debug-runtime-base64-test)

    add_executable(
        umicom-debug-runtime-capabilities-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_capabilities.c"
    )
    target_include_directories(
        umicom-debug-runtime-capabilities-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-capabilities-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-capabilities-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-capabilities-test)
    endif()
    add_test(NAME framework.debug_runtime.capabilities COMMAND umicom-debug-runtime-capabilities-test)

    add_executable(
        umicom-debug-runtime-command-router-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_command_router.c"
    )
    target_include_directories(
        umicom-debug-runtime-command-router-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-command-router-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-command-router-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-command-router-test)
    endif()
    add_test(NAME framework.debug_runtime.command-router COMMAND umicom-debug-runtime-command-router-test)

    add_executable(
        umicom-debug-runtime-contract-adapter-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_contract_adapter.c"
    )
    target_include_directories(
        umicom-debug-runtime-contract-adapter-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-contract-adapter-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-contract-adapter-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-contract-adapter-test)
    endif()
    add_test(NAME framework.debug_runtime.contract-adapter COMMAND umicom-debug-runtime-contract-adapter-test)

    add_executable(
        umicom-debug-runtime-decoder-breakpoints-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_decoder_breakpoints.c"
    )
    target_include_directories(
        umicom-debug-runtime-decoder-breakpoints-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-decoder-breakpoints-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-decoder-breakpoints-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-decoder-breakpoints-test)
    endif()
    add_test(NAME framework.debug_runtime.decoder-breakpoints COMMAND umicom-debug-runtime-decoder-breakpoints-test)

    add_executable(
        umicom-debug-runtime-decoder-completions-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_decoder_completions.c"
    )
    target_include_directories(
        umicom-debug-runtime-decoder-completions-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-decoder-completions-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-decoder-completions-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-decoder-completions-test)
    endif()
    add_test(NAME framework.debug_runtime.decoder-completions COMMAND umicom-debug-runtime-decoder-completions-test)

    add_executable(
        umicom-debug-runtime-decoder-data-breakpoint-info-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_decoder_data_breakpoint_info.c"
    )
    target_include_directories(
        umicom-debug-runtime-decoder-data-breakpoint-info-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-decoder-data-breakpoint-info-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-decoder-data-breakpoint-info-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-decoder-data-breakpoint-info-test)
    endif()
    add_test(NAME framework.debug_runtime.decoder-data-breakpoint-info COMMAND umicom-debug-runtime-decoder-data-breakpoint-info-test)

    add_executable(
        umicom-debug-runtime-decoder-disassembly-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_decoder_disassembly.c"
    )
    target_include_directories(
        umicom-debug-runtime-decoder-disassembly-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-decoder-disassembly-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-decoder-disassembly-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-decoder-disassembly-test)
    endif()
    add_test(NAME framework.debug_runtime.decoder-disassembly COMMAND umicom-debug-runtime-decoder-disassembly-test)

    add_executable(
        umicom-debug-runtime-decoder-evaluate-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_decoder_evaluate.c"
    )
    target_include_directories(
        umicom-debug-runtime-decoder-evaluate-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-decoder-evaluate-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-decoder-evaluate-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-decoder-evaluate-test)
    endif()
    add_test(NAME framework.debug_runtime.decoder-evaluate COMMAND umicom-debug-runtime-decoder-evaluate-test)

    add_executable(
        umicom-debug-runtime-decoder-event-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_decoder_event.c"
    )
    target_include_directories(
        umicom-debug-runtime-decoder-event-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-decoder-event-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-decoder-event-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-decoder-event-test)
    endif()
    add_test(NAME framework.debug_runtime.decoder-event COMMAND umicom-debug-runtime-decoder-event-test)

    add_executable(
        umicom-debug-runtime-decoder-exception-info-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_decoder_exception_info.c"
    )
    target_include_directories(
        umicom-debug-runtime-decoder-exception-info-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-decoder-exception-info-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-decoder-exception-info-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-decoder-exception-info-test)
    endif()
    add_test(NAME framework.debug_runtime.decoder-exception-info COMMAND umicom-debug-runtime-decoder-exception-info-test)

    add_executable(
        umicom-debug-runtime-decoder-initialize-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_decoder_initialize.c"
    )
    target_include_directories(
        umicom-debug-runtime-decoder-initialize-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-decoder-initialize-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-decoder-initialize-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-decoder-initialize-test)
    endif()
    add_test(NAME framework.debug_runtime.decoder-initialize COMMAND umicom-debug-runtime-decoder-initialize-test)

    add_executable(
        umicom-debug-runtime-decoder-memory-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_decoder_memory.c"
    )
    target_include_directories(
        umicom-debug-runtime-decoder-memory-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-decoder-memory-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-decoder-memory-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-decoder-memory-test)
    endif()
    add_test(NAME framework.debug_runtime.decoder-memory COMMAND umicom-debug-runtime-decoder-memory-test)

    add_executable(
        umicom-debug-runtime-decoder-modules-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_decoder_modules.c"
    )
    target_include_directories(
        umicom-debug-runtime-decoder-modules-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-decoder-modules-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-decoder-modules-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-decoder-modules-test)
    endif()
    add_test(NAME framework.debug_runtime.decoder-modules COMMAND umicom-debug-runtime-decoder-modules-test)

    add_executable(
        umicom-debug-runtime-decoder-scopes-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_decoder_scopes.c"
    )
    target_include_directories(
        umicom-debug-runtime-decoder-scopes-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-decoder-scopes-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-decoder-scopes-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-decoder-scopes-test)
    endif()
    add_test(NAME framework.debug_runtime.decoder-scopes COMMAND umicom-debug-runtime-decoder-scopes-test)

    add_executable(
        umicom-debug-runtime-decoder-source-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_decoder_source.c"
    )
    target_include_directories(
        umicom-debug-runtime-decoder-source-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-decoder-source-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-decoder-source-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-decoder-source-test)
    endif()
    add_test(NAME framework.debug_runtime.decoder-source COMMAND umicom-debug-runtime-decoder-source-test)

    add_executable(
        umicom-debug-runtime-decoder-stack-trace-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_decoder_stack_trace.c"
    )
    target_include_directories(
        umicom-debug-runtime-decoder-stack-trace-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-decoder-stack-trace-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-decoder-stack-trace-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-decoder-stack-trace-test)
    endif()
    add_test(NAME framework.debug_runtime.decoder-stack-trace COMMAND umicom-debug-runtime-decoder-stack-trace-test)

    add_executable(
        umicom-debug-runtime-decoder-threads-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_decoder_threads.c"
    )
    target_include_directories(
        umicom-debug-runtime-decoder-threads-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-decoder-threads-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-decoder-threads-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-decoder-threads-test)
    endif()
    add_test(NAME framework.debug_runtime.decoder-threads COMMAND umicom-debug-runtime-decoder-threads-test)

    add_executable(
        umicom-debug-runtime-decoder-variables-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_decoder_variables.c"
    )
    target_include_directories(
        umicom-debug-runtime-decoder-variables-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-decoder-variables-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-decoder-variables-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-decoder-variables-test)
    endif()
    add_test(NAME framework.debug_runtime.decoder-variables COMMAND umicom-debug-runtime-decoder-variables-test)

    add_executable(
        umicom-debug-runtime-event-queue-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_event_queue.c"
    )
    target_include_directories(
        umicom-debug-runtime-event-queue-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-event-queue-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-event-queue-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-event-queue-test)
    endif()
    add_test(NAME framework.debug_runtime.event-queue COMMAND umicom-debug-runtime-event-queue-test)

    add_executable(
        umicom-debug-runtime-message-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_message.c"
    )
    target_include_directories(
        umicom-debug-runtime-message-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-message-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-message-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-message-test)
    endif()
    add_test(NAME framework.debug_runtime.message COMMAND umicom-debug-runtime-message-test)

    add_executable(
        umicom-debug-runtime-pending-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_pending.c"
    )
    target_include_directories(
        umicom-debug-runtime-pending-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-pending-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-pending-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-pending-test)
    endif()
    add_test(NAME framework.debug_runtime.pending COMMAND umicom-debug-runtime-pending-test)

    add_executable(
        umicom-debug-runtime-platform-create-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_platform_create.c"
    )
    target_include_directories(
        umicom-debug-runtime-platform-create-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-platform-create-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-platform-create-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-platform-create-test)
    endif()
    add_test(NAME framework.debug_runtime.platform-create COMMAND umicom-debug-runtime-platform-create-test)

    add_executable(
        umicom-debug-runtime-profile-codelldb-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_profile_codelldb.c"
    )
    target_include_directories(
        umicom-debug-runtime-profile-codelldb-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-profile-codelldb-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-profile-codelldb-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-profile-codelldb-test)
    endif()
    add_test(NAME framework.debug_runtime.profile-codelldb COMMAND umicom-debug-runtime-profile-codelldb-test)

    add_executable(
        umicom-debug-runtime-profile-cppvsdbg-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_profile_cppvsdbg.c"
    )
    target_include_directories(
        umicom-debug-runtime-profile-cppvsdbg-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-profile-cppvsdbg-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-profile-cppvsdbg-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-profile-cppvsdbg-test)
    endif()
    add_test(NAME framework.debug_runtime.profile-cppvsdbg COMMAND umicom-debug-runtime-profile-cppvsdbg-test)

    add_executable(
        umicom-debug-runtime-profile-debugpy-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_profile_debugpy.c"
    )
    target_include_directories(
        umicom-debug-runtime-profile-debugpy-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-profile-debugpy-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-profile-debugpy-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-profile-debugpy-test)
    endif()
    add_test(NAME framework.debug_runtime.profile-debugpy COMMAND umicom-debug-runtime-profile-debugpy-test)

    add_executable(
        umicom-debug-runtime-profile-gdb-dap-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_profile_gdb_dap.c"
    )
    target_include_directories(
        umicom-debug-runtime-profile-gdb-dap-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-profile-gdb-dap-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-profile-gdb-dap-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-profile-gdb-dap-test)
    endif()
    add_test(NAME framework.debug_runtime.profile-gdb-dap COMMAND umicom-debug-runtime-profile-gdb-dap-test)

    add_executable(
        umicom-debug-runtime-profile-health-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_profile_health.c"
    )
    target_include_directories(
        umicom-debug-runtime-profile-health-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-profile-health-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-profile-health-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-profile-health-test)
    endif()
    add_test(NAME framework.debug_runtime.profile-health COMMAND umicom-debug-runtime-profile-health-test)

    add_executable(
        umicom-debug-runtime-profile-java-debug-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_profile_java_debug.c"
    )
    target_include_directories(
        umicom-debug-runtime-profile-java-debug-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-profile-java-debug-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-profile-java-debug-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-profile-java-debug-test)
    endif()
    add_test(NAME framework.debug_runtime.profile-java-debug COMMAND umicom-debug-runtime-profile-java-debug-test)

    add_executable(
        umicom-debug-runtime-profile-js-debug-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_profile_js_debug.c"
    )
    target_include_directories(
        umicom-debug-runtime-profile-js-debug-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-profile-js-debug-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-profile-js-debug-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-profile-js-debug-test)
    endif()
    add_test(NAME framework.debug_runtime.profile-js-debug COMMAND umicom-debug-runtime-profile-js-debug-test)

    add_executable(
        umicom-debug-runtime-profile-lldb-dap-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_profile_lldb_dap.c"
    )
    target_include_directories(
        umicom-debug-runtime-profile-lldb-dap-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-profile-lldb-dap-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-profile-lldb-dap-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-profile-lldb-dap-test)
    endif()
    add_test(NAME framework.debug_runtime.profile-lldb-dap COMMAND umicom-debug-runtime-profile-lldb-dap-test)

    add_executable(
        umicom-debug-runtime-profile-netcoredbg-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_profile_netcoredbg.c"
    )
    target_include_directories(
        umicom-debug-runtime-profile-netcoredbg-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-profile-netcoredbg-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-profile-netcoredbg-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-profile-netcoredbg-test)
    endif()
    add_test(NAME framework.debug_runtime.profile-netcoredbg COMMAND umicom-debug-runtime-profile-netcoredbg-test)

    add_executable(
        umicom-debug-runtime-profile-php-debug-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_profile_php_debug.c"
    )
    target_include_directories(
        umicom-debug-runtime-profile-php-debug-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-profile-php-debug-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-profile-php-debug-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-profile-php-debug-test)
    endif()
    add_test(NAME framework.debug_runtime.profile-php-debug COMMAND umicom-debug-runtime-profile-php-debug-test)

    add_executable(
        umicom-debug-runtime-request-attach-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_request_attach.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/request_test_support.c"
    )
    target_include_directories(
        umicom-debug-runtime-request-attach-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-request-attach-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-request-attach-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-request-attach-test)
    endif()
    add_test(NAME framework.debug_runtime.request-attach COMMAND umicom-debug-runtime-request-attach-test)

    add_executable(
        umicom-debug-runtime-request-breakpoint-locations-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_request_breakpoint_locations.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/request_test_support.c"
    )
    target_include_directories(
        umicom-debug-runtime-request-breakpoint-locations-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-request-breakpoint-locations-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-request-breakpoint-locations-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-request-breakpoint-locations-test)
    endif()
    add_test(NAME framework.debug_runtime.request-breakpoint-locations COMMAND umicom-debug-runtime-request-breakpoint-locations-test)

    add_executable(
        umicom-debug-runtime-request-cancel-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_request_cancel.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/request_test_support.c"
    )
    target_include_directories(
        umicom-debug-runtime-request-cancel-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-request-cancel-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-request-cancel-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-request-cancel-test)
    endif()
    add_test(NAME framework.debug_runtime.request-cancel COMMAND umicom-debug-runtime-request-cancel-test)

    add_executable(
        umicom-debug-runtime-request-completions-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_request_completions.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/request_test_support.c"
    )
    target_include_directories(
        umicom-debug-runtime-request-completions-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-request-completions-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-request-completions-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-request-completions-test)
    endif()
    add_test(NAME framework.debug_runtime.request-completions COMMAND umicom-debug-runtime-request-completions-test)

    add_executable(
        umicom-debug-runtime-request-configuration-done-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_request_configuration_done.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/request_test_support.c"
    )
    target_include_directories(
        umicom-debug-runtime-request-configuration-done-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-request-configuration-done-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-request-configuration-done-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-request-configuration-done-test)
    endif()
    add_test(NAME framework.debug_runtime.request-configuration-done COMMAND umicom-debug-runtime-request-configuration-done-test)

    add_executable(
        umicom-debug-runtime-request-continue-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_request_continue.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/request_test_support.c"
    )
    target_include_directories(
        umicom-debug-runtime-request-continue-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-request-continue-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-request-continue-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-request-continue-test)
    endif()
    add_test(NAME framework.debug_runtime.request-continue COMMAND umicom-debug-runtime-request-continue-test)

    add_executable(
        umicom-debug-runtime-request-data-breakpoint-info-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_request_data_breakpoint_info.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/request_test_support.c"
    )
    target_include_directories(
        umicom-debug-runtime-request-data-breakpoint-info-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-request-data-breakpoint-info-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-request-data-breakpoint-info-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-request-data-breakpoint-info-test)
    endif()
    add_test(NAME framework.debug_runtime.request-data-breakpoint-info COMMAND umicom-debug-runtime-request-data-breakpoint-info-test)

    add_executable(
        umicom-debug-runtime-request-disassemble-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_request_disassemble.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/request_test_support.c"
    )
    target_include_directories(
        umicom-debug-runtime-request-disassemble-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-request-disassemble-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-request-disassemble-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-request-disassemble-test)
    endif()
    add_test(NAME framework.debug_runtime.request-disassemble COMMAND umicom-debug-runtime-request-disassemble-test)

    add_executable(
        umicom-debug-runtime-request-disconnect-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_request_disconnect.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/request_test_support.c"
    )
    target_include_directories(
        umicom-debug-runtime-request-disconnect-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-request-disconnect-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-request-disconnect-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-request-disconnect-test)
    endif()
    add_test(NAME framework.debug_runtime.request-disconnect COMMAND umicom-debug-runtime-request-disconnect-test)

    add_executable(
        umicom-debug-runtime-request-evaluate-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_request_evaluate.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/request_test_support.c"
    )
    target_include_directories(
        umicom-debug-runtime-request-evaluate-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-request-evaluate-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-request-evaluate-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-request-evaluate-test)
    endif()
    add_test(NAME framework.debug_runtime.request-evaluate COMMAND umicom-debug-runtime-request-evaluate-test)

    add_executable(
        umicom-debug-runtime-request-exception-info-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_request_exception_info.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/request_test_support.c"
    )
    target_include_directories(
        umicom-debug-runtime-request-exception-info-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-request-exception-info-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-request-exception-info-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-request-exception-info-test)
    endif()
    add_test(NAME framework.debug_runtime.request-exception-info COMMAND umicom-debug-runtime-request-exception-info-test)

    add_executable(
        umicom-debug-runtime-request-goto-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_request_goto.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/request_test_support.c"
    )
    target_include_directories(
        umicom-debug-runtime-request-goto-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-request-goto-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-request-goto-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-request-goto-test)
    endif()
    add_test(NAME framework.debug_runtime.request-goto COMMAND umicom-debug-runtime-request-goto-test)

    add_executable(
        umicom-debug-runtime-request-goto-targets-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_request_goto_targets.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/request_test_support.c"
    )
    target_include_directories(
        umicom-debug-runtime-request-goto-targets-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-request-goto-targets-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-request-goto-targets-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-request-goto-targets-test)
    endif()
    add_test(NAME framework.debug_runtime.request-goto-targets COMMAND umicom-debug-runtime-request-goto-targets-test)

    add_executable(
        umicom-debug-runtime-request-initialize-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_request_initialize.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/request_test_support.c"
    )
    target_include_directories(
        umicom-debug-runtime-request-initialize-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-request-initialize-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-request-initialize-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-request-initialize-test)
    endif()
    add_test(NAME framework.debug_runtime.request-initialize COMMAND umicom-debug-runtime-request-initialize-test)

    add_executable(
        umicom-debug-runtime-request-launch-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_request_launch.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/request_test_support.c"
    )
    target_include_directories(
        umicom-debug-runtime-request-launch-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-request-launch-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-request-launch-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-request-launch-test)
    endif()
    add_test(NAME framework.debug_runtime.request-launch COMMAND umicom-debug-runtime-request-launch-test)

    add_executable(
        umicom-debug-runtime-request-loaded-sources-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_request_loaded_sources.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/request_test_support.c"
    )
    target_include_directories(
        umicom-debug-runtime-request-loaded-sources-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-request-loaded-sources-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-request-loaded-sources-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-request-loaded-sources-test)
    endif()
    add_test(NAME framework.debug_runtime.request-loaded-sources COMMAND umicom-debug-runtime-request-loaded-sources-test)

    add_executable(
        umicom-debug-runtime-request-modules-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_request_modules.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/request_test_support.c"
    )
    target_include_directories(
        umicom-debug-runtime-request-modules-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-request-modules-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-request-modules-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-request-modules-test)
    endif()
    add_test(NAME framework.debug_runtime.request-modules COMMAND umicom-debug-runtime-request-modules-test)

    add_executable(
        umicom-debug-runtime-request-next-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_request_next.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/request_test_support.c"
    )
    target_include_directories(
        umicom-debug-runtime-request-next-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-request-next-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-request-next-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-request-next-test)
    endif()
    add_test(NAME framework.debug_runtime.request-next COMMAND umicom-debug-runtime-request-next-test)

    add_executable(
        umicom-debug-runtime-request-pause-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_request_pause.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/request_test_support.c"
    )
    target_include_directories(
        umicom-debug-runtime-request-pause-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-request-pause-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-request-pause-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-request-pause-test)
    endif()
    add_test(NAME framework.debug_runtime.request-pause COMMAND umicom-debug-runtime-request-pause-test)

    add_executable(
        umicom-debug-runtime-request-read-memory-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_request_read_memory.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/request_test_support.c"
    )
    target_include_directories(
        umicom-debug-runtime-request-read-memory-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-request-read-memory-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-request-read-memory-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-request-read-memory-test)
    endif()
    add_test(NAME framework.debug_runtime.request-read-memory COMMAND umicom-debug-runtime-request-read-memory-test)

    add_executable(
        umicom-debug-runtime-request-restart-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_request_restart.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/request_test_support.c"
    )
    target_include_directories(
        umicom-debug-runtime-request-restart-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-request-restart-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-request-restart-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-request-restart-test)
    endif()
    add_test(NAME framework.debug_runtime.request-restart COMMAND umicom-debug-runtime-request-restart-test)

    add_executable(
        umicom-debug-runtime-request-restart-frame-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_request_restart_frame.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/request_test_support.c"
    )
    target_include_directories(
        umicom-debug-runtime-request-restart-frame-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-request-restart-frame-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-request-restart-frame-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-request-restart-frame-test)
    endif()
    add_test(NAME framework.debug_runtime.request-restart-frame COMMAND umicom-debug-runtime-request-restart-frame-test)

    add_executable(
        umicom-debug-runtime-request-reverse-continue-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_request_reverse_continue.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/request_test_support.c"
    )
    target_include_directories(
        umicom-debug-runtime-request-reverse-continue-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-request-reverse-continue-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-request-reverse-continue-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-request-reverse-continue-test)
    endif()
    add_test(NAME framework.debug_runtime.request-reverse-continue COMMAND umicom-debug-runtime-request-reverse-continue-test)

    add_executable(
        umicom-debug-runtime-request-scopes-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_request_scopes.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/request_test_support.c"
    )
    target_include_directories(
        umicom-debug-runtime-request-scopes-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-request-scopes-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-request-scopes-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-request-scopes-test)
    endif()
    add_test(NAME framework.debug_runtime.request-scopes COMMAND umicom-debug-runtime-request-scopes-test)

    add_executable(
        umicom-debug-runtime-request-set-breakpoints-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_request_set_breakpoints.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/request_test_support.c"
    )
    target_include_directories(
        umicom-debug-runtime-request-set-breakpoints-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-request-set-breakpoints-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-request-set-breakpoints-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-request-set-breakpoints-test)
    endif()
    add_test(NAME framework.debug_runtime.request-set-breakpoints COMMAND umicom-debug-runtime-request-set-breakpoints-test)

    add_executable(
        umicom-debug-runtime-request-set-data-breakpoints-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_request_set_data_breakpoints.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/request_test_support.c"
    )
    target_include_directories(
        umicom-debug-runtime-request-set-data-breakpoints-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-request-set-data-breakpoints-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-request-set-data-breakpoints-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-request-set-data-breakpoints-test)
    endif()
    add_test(NAME framework.debug_runtime.request-set-data-breakpoints COMMAND umicom-debug-runtime-request-set-data-breakpoints-test)

    add_executable(
        umicom-debug-runtime-request-set-exception-breakpoints-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_request_set_exception_breakpoints.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/request_test_support.c"
    )
    target_include_directories(
        umicom-debug-runtime-request-set-exception-breakpoints-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-request-set-exception-breakpoints-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-request-set-exception-breakpoints-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-request-set-exception-breakpoints-test)
    endif()
    add_test(NAME framework.debug_runtime.request-set-exception-breakpoints COMMAND umicom-debug-runtime-request-set-exception-breakpoints-test)

    add_executable(
        umicom-debug-runtime-request-set-expression-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_request_set_expression.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/request_test_support.c"
    )
    target_include_directories(
        umicom-debug-runtime-request-set-expression-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-request-set-expression-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-request-set-expression-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-request-set-expression-test)
    endif()
    add_test(NAME framework.debug_runtime.request-set-expression COMMAND umicom-debug-runtime-request-set-expression-test)

    add_executable(
        umicom-debug-runtime-request-set-function-breakpoints-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_request_set_function_breakpoints.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/request_test_support.c"
    )
    target_include_directories(
        umicom-debug-runtime-request-set-function-breakpoints-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-request-set-function-breakpoints-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-request-set-function-breakpoints-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-request-set-function-breakpoints-test)
    endif()
    add_test(NAME framework.debug_runtime.request-set-function-breakpoints COMMAND umicom-debug-runtime-request-set-function-breakpoints-test)

    add_executable(
        umicom-debug-runtime-request-set-instruction-breakpoints-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_request_set_instruction_breakpoints.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/request_test_support.c"
    )
    target_include_directories(
        umicom-debug-runtime-request-set-instruction-breakpoints-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-request-set-instruction-breakpoints-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-request-set-instruction-breakpoints-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-request-set-instruction-breakpoints-test)
    endif()
    add_test(NAME framework.debug_runtime.request-set-instruction-breakpoints COMMAND umicom-debug-runtime-request-set-instruction-breakpoints-test)

    add_executable(
        umicom-debug-runtime-request-set-variable-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_request_set_variable.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/request_test_support.c"
    )
    target_include_directories(
        umicom-debug-runtime-request-set-variable-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-request-set-variable-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-request-set-variable-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-request-set-variable-test)
    endif()
    add_test(NAME framework.debug_runtime.request-set-variable COMMAND umicom-debug-runtime-request-set-variable-test)

    add_executable(
        umicom-debug-runtime-request-source-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_request_source.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/request_test_support.c"
    )
    target_include_directories(
        umicom-debug-runtime-request-source-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-request-source-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-request-source-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-request-source-test)
    endif()
    add_test(NAME framework.debug_runtime.request-source COMMAND umicom-debug-runtime-request-source-test)

    add_executable(
        umicom-debug-runtime-request-stack-trace-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_request_stack_trace.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/request_test_support.c"
    )
    target_include_directories(
        umicom-debug-runtime-request-stack-trace-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-request-stack-trace-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-request-stack-trace-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-request-stack-trace-test)
    endif()
    add_test(NAME framework.debug_runtime.request-stack-trace COMMAND umicom-debug-runtime-request-stack-trace-test)

    add_executable(
        umicom-debug-runtime-request-step-back-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_request_step_back.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/request_test_support.c"
    )
    target_include_directories(
        umicom-debug-runtime-request-step-back-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-request-step-back-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-request-step-back-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-request-step-back-test)
    endif()
    add_test(NAME framework.debug_runtime.request-step-back COMMAND umicom-debug-runtime-request-step-back-test)

    add_executable(
        umicom-debug-runtime-request-step-in-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_request_step_in.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/request_test_support.c"
    )
    target_include_directories(
        umicom-debug-runtime-request-step-in-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-request-step-in-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-request-step-in-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-request-step-in-test)
    endif()
    add_test(NAME framework.debug_runtime.request-step-in COMMAND umicom-debug-runtime-request-step-in-test)

    add_executable(
        umicom-debug-runtime-request-step-in-targets-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_request_step_in_targets.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/request_test_support.c"
    )
    target_include_directories(
        umicom-debug-runtime-request-step-in-targets-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-request-step-in-targets-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-request-step-in-targets-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-request-step-in-targets-test)
    endif()
    add_test(NAME framework.debug_runtime.request-step-in-targets COMMAND umicom-debug-runtime-request-step-in-targets-test)

    add_executable(
        umicom-debug-runtime-request-step-out-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_request_step_out.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/request_test_support.c"
    )
    target_include_directories(
        umicom-debug-runtime-request-step-out-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-request-step-out-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-request-step-out-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-request-step-out-test)
    endif()
    add_test(NAME framework.debug_runtime.request-step-out COMMAND umicom-debug-runtime-request-step-out-test)

    add_executable(
        umicom-debug-runtime-request-terminate-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_request_terminate.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/request_test_support.c"
    )
    target_include_directories(
        umicom-debug-runtime-request-terminate-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-request-terminate-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-request-terminate-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-request-terminate-test)
    endif()
    add_test(NAME framework.debug_runtime.request-terminate COMMAND umicom-debug-runtime-request-terminate-test)

    add_executable(
        umicom-debug-runtime-request-terminate-threads-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_request_terminate_threads.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/request_test_support.c"
    )
    target_include_directories(
        umicom-debug-runtime-request-terminate-threads-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-request-terminate-threads-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-request-terminate-threads-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-request-terminate-threads-test)
    endif()
    add_test(NAME framework.debug_runtime.request-terminate-threads COMMAND umicom-debug-runtime-request-terminate-threads-test)

    add_executable(
        umicom-debug-runtime-request-threads-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_request_threads.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/request_test_support.c"
    )
    target_include_directories(
        umicom-debug-runtime-request-threads-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-request-threads-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-request-threads-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-request-threads-test)
    endif()
    add_test(NAME framework.debug_runtime.request-threads COMMAND umicom-debug-runtime-request-threads-test)

    add_executable(
        umicom-debug-runtime-request-variables-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_request_variables.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/request_test_support.c"
    )
    target_include_directories(
        umicom-debug-runtime-request-variables-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-request-variables-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-request-variables-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-request-variables-test)
    endif()
    add_test(NAME framework.debug_runtime.request-variables COMMAND umicom-debug-runtime-request-variables-test)

    add_executable(
        umicom-debug-runtime-request-write-memory-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_request_write_memory.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/request_test_support.c"
    )
    target_include_directories(
        umicom-debug-runtime-request-write-memory-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-request-write-memory-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-request-write-memory-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-request-write-memory-test)
    endif()
    add_test(NAME framework.debug_runtime.request-write-memory COMMAND umicom-debug-runtime-request-write-memory-test)

    add_executable(
        umicom-debug-runtime-response-queue-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_response_queue.c"
    )
    target_include_directories(
        umicom-debug-runtime-response-queue-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-response-queue-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-response-queue-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-response-queue-test)
    endif()
    add_test(NAME framework.debug_runtime.response-queue COMMAND umicom-debug-runtime-response-queue-test)

    add_executable(
        umicom-debug-runtime-service-bridge-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_service_bridge.c"
    )
    target_include_directories(
        umicom-debug-runtime-service-bridge-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-service-bridge-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-service-bridge-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-service-bridge-test)
    endif()
    add_test(NAME framework.debug_runtime.service-bridge COMMAND umicom-debug-runtime-service-bridge-test)

    add_executable(
        umicom-debug-runtime-types-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_types.c"
    )
    target_include_directories(
        umicom-debug-runtime-types-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-types-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-types-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-types-test)
    endif()
    add_test(NAME framework.debug_runtime.types COMMAND umicom-debug-runtime-types-test)

    add_executable(
        umicom-debug-runtime-workbench-bridge-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime/test_workbench_bridge.c"
    )
    target_include_directories(
        umicom-debug-runtime-workbench-bridge-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/debug_runtime"
    )
    target_link_libraries(umicom-debug-runtime-workbench-bridge-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-debug-runtime-workbench-bridge-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-debug-runtime-workbench-bridge-test)
    endif()
    add_test(NAME framework.debug_runtime.workbench-bridge COMMAND umicom-debug-runtime-workbench-bridge-test)

endif()

message(STATUS
    "Umicom real Debug Adapter Protocol runtime enabled")
