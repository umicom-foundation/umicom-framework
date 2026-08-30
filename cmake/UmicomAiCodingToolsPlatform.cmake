#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomAiCodingToolsPlatform.cmake
#
# PURPOSE:
#   Attach controlled AI developer tools, multi-step tool plans, checkpoints,
#   provider tool-call loops and repository-aware coding chat to the canonical
#   developer library.
#
# ARCHITECTURE:
#   Existing AI Runtime/ToolRegistry, DeveloperExecutor, Source Control,
#   Language Service, DAP runtime and governed AI Coding Runtime remain
#   authoritative. This layer exposes those capabilities to models only through
#   explicit tool descriptors, central policy and approval mediation.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)

if(NOT TARGET umicom_developer)
    message(FATAL_ERROR
        "AI Coding Tools require the canonical umicom_developer target")
endif()

if(NOT TARGET Umicom::ai)
    message(FATAL_ERROR
        "AI Coding Tools require the existing Umicom::ai target")
endif()

target_sources(umicom_developer PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_tools/agent_loop.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_tools/call_parser.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_tools/catalogue.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_tools/chat_registry.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_tools/chat_session.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_tools/checkpoint.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_tools/environment.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_tools/executor.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_tools/json.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_tools/manifest.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_tools/plan.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_tools/plan_executor.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_tools/plan_parser.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_tools/platform.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_tools/policy.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_tools/process_tool_support.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_tools/prompt.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_tools/result_history.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_tools/tool_support.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_tools/tools/agent_apply.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_tools/tools/agent_approve.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_tools/tools/agent_cancel.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_tools/tools/agent_continue_repair.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_tools/tools/agent_reject.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_tools/tools/agent_snapshot.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_tools/tools/checkpoint_create.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_tools/tools/checkpoint_list.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_tools/tools/checkpoint_restore.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_tools/tools/debug_continue.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_tools/tools/debug_pause.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_tools/tools/debug_snapshot.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_tools/tools/debug_stack.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_tools/tools/debug_step_into.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_tools/tools/debug_step_out.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_tools/tools/debug_step_over.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_tools/tools/debug_threads.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_tools/tools/developer_build.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_tools/tools/developer_configure.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_tools/tools/developer_package.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_tools/tools/developer_run.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_tools/tools/developer_test.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_tools/tools/language_diagnostics.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_tools/tools/language_symbol_find.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_tools/tools/language_symbols.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_tools/tools/source_control_commit.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_tools/tools/source_control_diff.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_tools/tools/source_control_fetch.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_tools/tools/source_control_pull.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_tools/tools/source_control_push.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_tools/tools/source_control_stage.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_tools/tools/source_control_stage_all.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_tools/tools/source_control_status.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_tools/tools/source_control_unstage.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_tools/tools/tools_catalogue.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_tools/tools/tools_policy.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_tools/tools/workspace_exists.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_tools/tools/workspace_list.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_tools/tools/workspace_read.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_tools/tools/workspace_search.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_tools/types.c"
)

# The generic AI runtime owns provider/tool registries; coding-tool execution
# remains a developer capability while preserving static-link closure.
target_link_libraries(umicom_developer PUBLIC
    Umicom::ai
)

if(BUILD_TESTING)
    add_executable(
        umicom-ai-coding-tools-agent-loop-approval-stop-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_tools/test_agent_loop_approval_stop.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_tools/tool_test_support.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime/workspace_test_support.c"
    )
    target_include_directories(
        umicom-ai-coding-tools-agent-loop-approval-stop-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_tools"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime"
    )
    target_link_libraries(umicom-ai-coding-tools-agent-loop-approval-stop-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-coding-tools-agent-loop-approval-stop-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-coding-tools-agent-loop-approval-stop-test)
    endif()
    add_test(NAME framework.ai_coding_tools.agent-loop-approval-stop COMMAND umicom-ai-coding-tools-agent-loop-approval-stop-test)

    add_executable(
        umicom-ai-coding-tools-agent-loop-final-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_tools/test_agent_loop_final.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_tools/tool_test_support.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime/workspace_test_support.c"
    )
    target_include_directories(
        umicom-ai-coding-tools-agent-loop-final-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_tools"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime"
    )
    target_link_libraries(umicom-ai-coding-tools-agent-loop-final-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-coding-tools-agent-loop-final-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-coding-tools-agent-loop-final-test)
    endif()
    add_test(NAME framework.ai_coding_tools.agent-loop-final COMMAND umicom-ai-coding-tools-agent-loop-final-test)

    add_executable(
        umicom-ai-coding-tools-agent-loop-plan-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_tools/test_agent_loop_plan.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_tools/tool_test_support.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime/workspace_test_support.c"
    )
    target_include_directories(
        umicom-ai-coding-tools-agent-loop-plan-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_tools"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime"
    )
    target_link_libraries(umicom-ai-coding-tools-agent-loop-plan-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-coding-tools-agent-loop-plan-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-coding-tools-agent-loop-plan-test)
    endif()
    add_test(NAME framework.ai_coding_tools.agent-loop-plan COMMAND umicom-ai-coding-tools-agent-loop-plan-test)

    add_executable(
        umicom-ai-coding-tools-agent-loop-tool-call-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_tools/test_agent_loop_tool_call.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_tools/tool_test_support.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime/workspace_test_support.c"
    )
    target_include_directories(
        umicom-ai-coding-tools-agent-loop-tool-call-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_tools"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime"
    )
    target_link_libraries(umicom-ai-coding-tools-agent-loop-tool-call-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-coding-tools-agent-loop-tool-call-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-coding-tools-agent-loop-tool-call-test)
    endif()
    add_test(NAME framework.ai_coding_tools.agent-loop-tool-call COMMAND umicom-ai-coding-tools-agent-loop-tool-call-test)

    add_executable(
        umicom-ai-coding-tools-call-parser-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_tools/test_call_parser.c"
    )
    target_include_directories(
        umicom-ai-coding-tools-call-parser-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_tools"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime"
    )
    target_link_libraries(umicom-ai-coding-tools-call-parser-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-coding-tools-call-parser-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-coding-tools-call-parser-test)
    endif()
    add_test(NAME framework.ai_coding_tools.call-parser COMMAND umicom-ai-coding-tools-call-parser-test)

    add_executable(
        umicom-ai-coding-tools-catalogue-manifest-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_tools/test_catalogue_manifest.c"
    )
    target_include_directories(
        umicom-ai-coding-tools-catalogue-manifest-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_tools"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime"
    )
    target_link_libraries(umicom-ai-coding-tools-catalogue-manifest-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-coding-tools-catalogue-manifest-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-coding-tools-catalogue-manifest-test)
    endif()
    add_test(NAME framework.ai_coding_tools.catalogue-manifest COMMAND umicom-ai-coding-tools-catalogue-manifest-test)

    add_executable(
        umicom-ai-coding-tools-chat-registry-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_tools/test_chat_registry.c"
    )
    target_include_directories(
        umicom-ai-coding-tools-chat-registry-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_tools"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime"
    )
    target_link_libraries(umicom-ai-coding-tools-chat-registry-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-coding-tools-chat-registry-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-coding-tools-chat-registry-test)
    endif()
    add_test(NAME framework.ai_coding_tools.chat-registry COMMAND umicom-ai-coding-tools-chat-registry-test)

    add_executable(
        umicom-ai-coding-tools-chat-session-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_tools/test_chat_session.c"
    )
    target_include_directories(
        umicom-ai-coding-tools-chat-session-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_tools"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime"
    )
    target_link_libraries(umicom-ai-coding-tools-chat-session-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-coding-tools-chat-session-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-coding-tools-chat-session-test)
    endif()
    add_test(NAME framework.ai_coding_tools.chat-session COMMAND umicom-ai-coding-tools-chat-session-test)

    add_executable(
        umicom-ai-coding-tools-checkpoint-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_tools/test_checkpoint.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_tools/tool_test_support.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime/workspace_test_support.c"
    )
    target_include_directories(
        umicom-ai-coding-tools-checkpoint-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_tools"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime"
    )
    target_link_libraries(umicom-ai-coding-tools-checkpoint-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-coding-tools-checkpoint-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-coding-tools-checkpoint-test)
    endif()
    add_test(NAME framework.ai_coding_tools.checkpoint COMMAND umicom-ai-coding-tools-checkpoint-test)

    add_executable(
        umicom-ai-coding-tools-checkpoint-tools-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_tools/test_checkpoint_tools.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_tools/tool_test_support.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime/workspace_test_support.c"
    )
    target_include_directories(
        umicom-ai-coding-tools-checkpoint-tools-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_tools"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime"
    )
    target_link_libraries(umicom-ai-coding-tools-checkpoint-tools-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-coding-tools-checkpoint-tools-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-coding-tools-checkpoint-tools-test)
    endif()
    add_test(NAME framework.ai_coding_tools.checkpoint-tools COMMAND umicom-ai-coding-tools-checkpoint-tools-test)

    add_executable(
        umicom-ai-coding-tools-compound-network-policy-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_tools/test_compound_network_policy.c"
    )
    target_include_directories(
        umicom-ai-coding-tools-compound-network-policy-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_tools"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime"
    )
    target_link_libraries(umicom-ai-coding-tools-compound-network-policy-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-coding-tools-compound-network-policy-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-coding-tools-compound-network-policy-test)
    endif()
    add_test(NAME framework.ai_coding_tools.compound-network-policy COMMAND umicom-ai-coding-tools-compound-network-policy-test)

    add_executable(
        umicom-ai-coding-tools-control-risk-policy-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_tools/test_control_risk_policy.c"
    )
    target_include_directories(
        umicom-ai-coding-tools-control-risk-policy-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_tools"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime"
    )
    target_link_libraries(umicom-ai-coding-tools-control-risk-policy-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-coding-tools-control-risk-policy-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-coding-tools-control-risk-policy-test)
    endif()
    add_test(NAME framework.ai_coding_tools.control-risk-policy COMMAND umicom-ai-coding-tools-control-risk-policy-test)

    add_executable(
        umicom-ai-coding-tools-language-diagnostics-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_tools/test_language_diagnostics.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_tools/tool_test_support.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime/workspace_test_support.c"
    )
    target_include_directories(
        umicom-ai-coding-tools-language-diagnostics-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_tools"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime"
    )
    target_link_libraries(umicom-ai-coding-tools-language-diagnostics-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-coding-tools-language-diagnostics-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-coding-tools-language-diagnostics-test)
    endif()
    add_test(NAME framework.ai_coding_tools.language-diagnostics COMMAND umicom-ai-coding-tools-language-diagnostics-test)

    add_executable(
        umicom-ai-coding-tools-language-symbols-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_tools/test_language_symbols.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_tools/tool_test_support.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime/workspace_test_support.c"
    )
    target_include_directories(
        umicom-ai-coding-tools-language-symbols-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_tools"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime"
    )
    target_link_libraries(umicom-ai-coding-tools-language-symbols-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-coding-tools-language-symbols-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-coding-tools-language-symbols-test)
    endif()
    add_test(NAME framework.ai_coding_tools.language-symbols COMMAND umicom-ai-coding-tools-language-symbols-test)

    add_executable(
        umicom-ai-coding-tools-plan-executor-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_tools/test_plan_executor.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_tools/tool_test_support.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime/workspace_test_support.c"
    )
    target_include_directories(
        umicom-ai-coding-tools-plan-executor-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_tools"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime"
    )
    target_link_libraries(umicom-ai-coding-tools-plan-executor-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-coding-tools-plan-executor-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-coding-tools-plan-executor-test)
    endif()
    add_test(NAME framework.ai_coding_tools.plan-executor COMMAND umicom-ai-coding-tools-plan-executor-test)

    add_executable(
        umicom-ai-coding-tools-plan-parser-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_tools/test_plan_parser.c"
    )
    target_include_directories(
        umicom-ai-coding-tools-plan-parser-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_tools"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime"
    )
    target_link_libraries(umicom-ai-coding-tools-plan-parser-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-coding-tools-plan-parser-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-coding-tools-plan-parser-test)
    endif()
    add_test(NAME framework.ai_coding_tools.plan-parser COMMAND umicom-ai-coding-tools-plan-parser-test)

    add_executable(
        umicom-ai-coding-tools-platform-chat-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_tools/test_platform_chat.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_tools/tool_test_support.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime/workspace_test_support.c"
    )
    target_include_directories(
        umicom-ai-coding-tools-platform-chat-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_tools"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime"
    )
    target_link_libraries(umicom-ai-coding-tools-platform-chat-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-coding-tools-platform-chat-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-coding-tools-platform-chat-test)
    endif()
    add_test(NAME framework.ai_coding_tools.platform-chat COMMAND umicom-ai-coding-tools-platform-chat-test)

    add_executable(
        umicom-ai-coding-tools-platform-execute-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_tools/test_platform_execute.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_tools/tool_test_support.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime/workspace_test_support.c"
    )
    target_include_directories(
        umicom-ai-coding-tools-platform-execute-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_tools"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime"
    )
    target_link_libraries(umicom-ai-coding-tools-platform-execute-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-coding-tools-platform-execute-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-coding-tools-platform-execute-test)
    endif()
    add_test(NAME framework.ai_coding_tools.platform-execute COMMAND umicom-ai-coding-tools-platform-execute-test)

    add_executable(
        umicom-ai-coding-tools-platform-policy-restore-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_tools/test_platform_policy_restore.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_tools/tool_test_support.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime/workspace_test_support.c"
    )
    target_include_directories(
        umicom-ai-coding-tools-platform-policy-restore-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_tools"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime"
    )
    target_link_libraries(umicom-ai-coding-tools-platform-policy-restore-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-coding-tools-platform-policy-restore-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-coding-tools-platform-policy-restore-test)
    endif()
    add_test(NAME framework.ai_coding_tools.platform-policy-restore COMMAND umicom-ai-coding-tools-platform-policy-restore-test)

    add_executable(
        umicom-ai-coding-tools-platform-snapshot-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_tools/test_platform_snapshot.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_tools/tool_test_support.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime/workspace_test_support.c"
    )
    target_include_directories(
        umicom-ai-coding-tools-platform-snapshot-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_tools"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime"
    )
    target_link_libraries(umicom-ai-coding-tools-platform-snapshot-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-coding-tools-platform-snapshot-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-coding-tools-platform-snapshot-test)
    endif()
    add_test(NAME framework.ai_coding_tools.platform-snapshot COMMAND umicom-ai-coding-tools-platform-snapshot-test)

    add_executable(
        umicom-ai-coding-tools-process-execution-policy-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_tools/test_process_execution_policy.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_tools/tool_test_support.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime/workspace_test_support.c"
    )
    target_include_directories(
        umicom-ai-coding-tools-process-execution-policy-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_tools"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime"
    )
    target_link_libraries(umicom-ai-coding-tools-process-execution-policy-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-coding-tools-process-execution-policy-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-coding-tools-process-execution-policy-test)
    endif()
    add_test(NAME framework.ai_coding_tools.process-execution-policy COMMAND umicom-ai-coding-tools-process-execution-policy-test)

    add_executable(
        umicom-ai-coding-tools-prompt-filtering-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_tools/test_prompt_filtering.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_tools/tool_test_support.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime/workspace_test_support.c"
    )
    target_include_directories(
        umicom-ai-coding-tools-prompt-filtering-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_tools"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime"
    )
    target_link_libraries(umicom-ai-coding-tools-prompt-filtering-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-coding-tools-prompt-filtering-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-coding-tools-prompt-filtering-test)
    endif()
    add_test(NAME framework.ai_coding_tools.prompt-filtering COMMAND umicom-ai-coding-tools-prompt-filtering-test)

    add_executable(
        umicom-ai-coding-tools-result-history-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_tools/test_result_history.c"
    )
    target_include_directories(
        umicom-ai-coding-tools-result-history-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_tools"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime"
    )
    target_link_libraries(umicom-ai-coding-tools-result-history-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-coding-tools-result-history-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-coding-tools-result-history-test)
    endif()
    add_test(NAME framework.ai_coding_tools.result-history COMMAND umicom-ai-coding-tools-result-history-test)

    add_executable(
        umicom-ai-coding-tools-tool-registration-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_tools/test_tool_registration.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_tools/tool_test_support.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime/workspace_test_support.c"
    )
    target_include_directories(
        umicom-ai-coding-tools-tool-registration-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_tools"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime"
    )
    target_link_libraries(umicom-ai-coding-tools-tool-registration-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-coding-tools-tool-registration-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-coding-tools-tool-registration-test)
    endif()
    add_test(NAME framework.ai_coding_tools.tool-registration COMMAND umicom-ai-coding-tools-tool-registration-test)

    add_executable(
        umicom-ai-coding-tools-types-policy-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_tools/test_types_policy.c"
    )
    target_include_directories(
        umicom-ai-coding-tools-types-policy-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_tools"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime"
    )
    target_link_libraries(umicom-ai-coding-tools-types-policy-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-coding-tools-types-policy-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-coding-tools-types-policy-test)
    endif()
    add_test(NAME framework.ai_coding_tools.types-policy COMMAND umicom-ai-coding-tools-types-policy-test)

    add_executable(
        umicom-ai-coding-tools-workspace-exists-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_tools/test_workspace_exists.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_tools/tool_test_support.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime/workspace_test_support.c"
    )
    target_include_directories(
        umicom-ai-coding-tools-workspace-exists-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_tools"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime"
    )
    target_link_libraries(umicom-ai-coding-tools-workspace-exists-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-coding-tools-workspace-exists-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-coding-tools-workspace-exists-test)
    endif()
    add_test(NAME framework.ai_coding_tools.workspace-exists COMMAND umicom-ai-coding-tools-workspace-exists-test)

    add_executable(
        umicom-ai-coding-tools-workspace-read-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_tools/test_workspace_read.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_tools/tool_test_support.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime/workspace_test_support.c"
    )
    target_include_directories(
        umicom-ai-coding-tools-workspace-read-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_tools"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime"
    )
    target_link_libraries(umicom-ai-coding-tools-workspace-read-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-coding-tools-workspace-read-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-coding-tools-workspace-read-test)
    endif()
    add_test(NAME framework.ai_coding_tools.workspace-read COMMAND umicom-ai-coding-tools-workspace-read-test)

endif()

message(STATUS
    "Umicom controlled AI developer tools and autonomous tool loop enabled")
