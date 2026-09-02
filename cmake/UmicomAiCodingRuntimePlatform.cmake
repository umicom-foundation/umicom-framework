#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomAiCodingRuntimePlatform.cmake
#
# PURPOSE:
#   Attach the operational repository-aware AI coding/vibe-coding runtime to the
#   canonical developer library and register focused regression coverage.
#
# ARCHITECTURE:
#   Existing AuthorEngine, AI provider runtime, coding context/patch governance,
#   Developer Workbench and Developer Executor remain authoritative. This layer
#   supplies real repository scanning, context materialisation, provider
#   execution, complete-file patch parsing, approval/apply/revert, build/test
#   validation, bounded repair loops and workbench command bindings.
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
        "AI Coding Runtime requires the canonical umicom_developer target")
endif()

# Load the dependency only when the parent build has not already provided its target.
if(NOT TARGET Umicom::ai)
    message(FATAL_ERROR
        "AI Coding Runtime requires the existing Umicom::ai target")
endif()

# Apply this branch only when its contract condition is satisfied.
if(WIN32)
    set(UMICOM_AI_CODING_SCANNER_PLATFORM_SOURCE
        "${CMAKE_CURRENT_LIST_DIR}/../adapters/windows/ai_coding_scanner_windows.c")
# Use this fallback path when the earlier condition does not apply.
else()
    set(UMICOM_AI_CODING_SCANNER_PLATFORM_SOURCE
        "${CMAKE_CURRENT_LIST_DIR}/../adapters/posix/ai_coding_scanner_posix.c")
endif()

target_sources(umicom_developer PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_runtime/agent.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_runtime/audit.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_runtime/cancellation.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_runtime/change_guard.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_runtime/config.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_runtime/context_materializer.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_runtime/context_summary.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_runtime/event_queue.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_runtime/history.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_runtime/ignore.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_runtime/language.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_runtime/local_workspace.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_runtime/patch_preview.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_runtime/path.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_runtime/platform.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_runtime/prompt.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_runtime/prompt_template.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_runtime/prompts/chat.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_runtime/prompts/complete.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_runtime/prompts/explain.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_runtime/prompts/generate_tests.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_runtime/prompts/refactor.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_runtime/provider_executor.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_runtime/repair_policy.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_runtime/repository_index.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_runtime/response.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_runtime/scanner.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_runtime/selection.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_runtime/session.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_runtime/task_queue.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_runtime/types.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_runtime/validation_feedback.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_runtime/validation_plan.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_runtime/validation_profiles/cargo.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_runtime/validation_profiles/cmake.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_runtime/validation_profiles/go.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_runtime/validation_profiles/gradle.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_runtime/validation_profiles/make.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_runtime/validation_profiles/maven.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_runtime/validation_profiles/meson.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_runtime/validation_profiles/npm.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_runtime/validation_profiles/python.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_runtime/validation_profiles/support.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_runtime/validation_profiles/zig.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_runtime/validation_runner.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_runtime/validation_selector.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_runtime/workbench_bridge.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_coding_runtime/workspace.c"
    ${UMICOM_AI_CODING_SCANNER_PLATFORM_SOURCE}
)

# Coding-agent execution is a developer capability but reuses the existing AI
# provider/AuthorEngine implementation. Keeping the dependency public preserves
# static-link closure for thin applications consuming Umicom::developer.
target_link_libraries(umicom_developer PUBLIC
    Umicom::ai
)

# Register verification targets only when the developer has enabled testing.
if(BUILD_TESTING)
    add_executable(
        umicom-ai-coding-runtime-audit-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime/test_audit.c"
    )
    target_include_directories(
        umicom-ai-coding-runtime-audit-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime"
    )
    target_link_libraries(umicom-ai-coding-runtime-audit-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-coding-runtime-audit-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-coding-runtime-audit-test)
    endif()
    add_test(NAME framework.ai_coding_runtime.audit COMMAND umicom-ai-coding-runtime-audit-test)

    add_executable(
        umicom-ai-coding-runtime-cancellation-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime/test_cancellation.c"
    )
    target_include_directories(
        umicom-ai-coding-runtime-cancellation-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime"
    )
    target_link_libraries(umicom-ai-coding-runtime-cancellation-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-coding-runtime-cancellation-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-coding-runtime-cancellation-test)
    endif()
    add_test(NAME framework.ai_coding_runtime.cancellation COMMAND umicom-ai-coding-runtime-cancellation-test)

    add_executable(
        umicom-ai-coding-runtime-change-guard-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime/test_change_guard.c"
    )
    target_include_directories(
        umicom-ai-coding-runtime-change-guard-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime"
    )
    target_link_libraries(umicom-ai-coding-runtime-change-guard-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-coding-runtime-change-guard-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-coding-runtime-change-guard-test)
    endif()
    add_test(NAME framework.ai_coding_runtime.change-guard COMMAND umicom-ai-coding-runtime-change-guard-test)

    add_executable(
        umicom-ai-coding-runtime-config-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime/test_config.c"
    )
    target_include_directories(
        umicom-ai-coding-runtime-config-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime"
    )
    target_link_libraries(umicom-ai-coding-runtime-config-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-coding-runtime-config-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-coding-runtime-config-test)
    endif()
    add_test(NAME framework.ai_coding_runtime.config COMMAND umicom-ai-coding-runtime-config-test)

    add_executable(
        umicom-ai-coding-runtime-context-materializer-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime/test_context_materializer.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime/workspace_test_support.c"
    )
    target_include_directories(
        umicom-ai-coding-runtime-context-materializer-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime"
    )
    target_link_libraries(umicom-ai-coding-runtime-context-materializer-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-coding-runtime-context-materializer-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-coding-runtime-context-materializer-test)
    endif()
    add_test(NAME framework.ai_coding_runtime.context-materializer COMMAND umicom-ai-coding-runtime-context-materializer-test)

    add_executable(
        umicom-ai-coding-runtime-context-summary-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime/test_context_summary.c"
    )
    target_include_directories(
        umicom-ai-coding-runtime-context-summary-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime"
    )
    target_link_libraries(umicom-ai-coding-runtime-context-summary-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-coding-runtime-context-summary-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-coding-runtime-context-summary-test)
    endif()
    add_test(NAME framework.ai_coding_runtime.context-summary COMMAND umicom-ai-coding-runtime-context-summary-test)

    add_executable(
        umicom-ai-coding-runtime-event-queue-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime/test_event_queue.c"
    )
    target_include_directories(
        umicom-ai-coding-runtime-event-queue-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime"
    )
    target_link_libraries(umicom-ai-coding-runtime-event-queue-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-coding-runtime-event-queue-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-coding-runtime-event-queue-test)
    endif()
    add_test(NAME framework.ai_coding_runtime.event-queue COMMAND umicom-ai-coding-runtime-event-queue-test)

    add_executable(
        umicom-ai-coding-runtime-history-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime/test_history.c"
    )
    target_include_directories(
        umicom-ai-coding-runtime-history-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime"
    )
    target_link_libraries(umicom-ai-coding-runtime-history-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-coding-runtime-history-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-coding-runtime-history-test)
    endif()
    add_test(NAME framework.ai_coding_runtime.history COMMAND umicom-ai-coding-runtime-history-test)

    add_executable(
        umicom-ai-coding-runtime-ignore-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime/test_ignore.c"
    )
    target_include_directories(
        umicom-ai-coding-runtime-ignore-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime"
    )
    target_link_libraries(umicom-ai-coding-runtime-ignore-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-coding-runtime-ignore-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-coding-runtime-ignore-test)
    endif()
    add_test(NAME framework.ai_coding_runtime.ignore COMMAND umicom-ai-coding-runtime-ignore-test)

    add_executable(
        umicom-ai-coding-runtime-language-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime/test_language.c"
    )
    target_include_directories(
        umicom-ai-coding-runtime-language-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime"
    )
    target_link_libraries(umicom-ai-coding-runtime-language-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-coding-runtime-language-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-coding-runtime-language-test)
    endif()
    add_test(NAME framework.ai_coding_runtime.language COMMAND umicom-ai-coding-runtime-language-test)

    add_executable(
        umicom-ai-coding-runtime-patch-preview-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime/test_patch_preview.c"
    )
    target_include_directories(
        umicom-ai-coding-runtime-patch-preview-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime"
    )
    target_link_libraries(umicom-ai-coding-runtime-patch-preview-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-coding-runtime-patch-preview-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-coding-runtime-patch-preview-test)
    endif()
    add_test(NAME framework.ai_coding_runtime.patch-preview COMMAND umicom-ai-coding-runtime-patch-preview-test)

    add_executable(
        umicom-ai-coding-runtime-path-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime/test_path.c"
    )
    target_include_directories(
        umicom-ai-coding-runtime-path-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime"
    )
    target_link_libraries(umicom-ai-coding-runtime-path-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-coding-runtime-path-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-coding-runtime-path-test)
    endif()
    add_test(NAME framework.ai_coding_runtime.path COMMAND umicom-ai-coding-runtime-path-test)

    add_executable(
        umicom-ai-coding-runtime-path-traversal-response-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime/test_path_traversal_response.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime/workspace_test_support.c"
    )
    target_include_directories(
        umicom-ai-coding-runtime-path-traversal-response-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime"
    )
    target_link_libraries(umicom-ai-coding-runtime-path-traversal-response-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-coding-runtime-path-traversal-response-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-coding-runtime-path-traversal-response-test)
    endif()
    add_test(NAME framework.ai_coding_runtime.path-traversal-response COMMAND umicom-ai-coding-runtime-path-traversal-response-test)

    add_executable(
        umicom-ai-coding-runtime-prompt-template-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime/test_prompt_template.c"
    )
    target_include_directories(
        umicom-ai-coding-runtime-prompt-template-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime"
    )
    target_link_libraries(umicom-ai-coding-runtime-prompt-template-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-coding-runtime-prompt-template-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-coding-runtime-prompt-template-test)
    endif()
    add_test(NAME framework.ai_coding_runtime.prompt-template COMMAND umicom-ai-coding-runtime-prompt-template-test)

    add_executable(
        umicom-ai-coding-runtime-repair-policy-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime/test_repair_policy.c"
    )
    target_include_directories(
        umicom-ai-coding-runtime-repair-policy-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime"
    )
    target_link_libraries(umicom-ai-coding-runtime-repair-policy-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-coding-runtime-repair-policy-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-coding-runtime-repair-policy-test)
    endif()
    add_test(NAME framework.ai_coding_runtime.repair-policy COMMAND umicom-ai-coding-runtime-repair-policy-test)

    add_executable(
        umicom-ai-coding-runtime-response-create-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime/test_response_create.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime/workspace_test_support.c"
    )
    target_include_directories(
        umicom-ai-coding-runtime-response-create-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime"
    )
    target_link_libraries(umicom-ai-coding-runtime-response-create-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-coding-runtime-response-create-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-coding-runtime-response-create-test)
    endif()
    add_test(NAME framework.ai_coding_runtime.response-create COMMAND umicom-ai-coding-runtime-response-create-test)

    add_executable(
        umicom-ai-coding-runtime-response-modify-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime/test_response_modify.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime/workspace_test_support.c"
    )
    target_include_directories(
        umicom-ai-coding-runtime-response-modify-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime"
    )
    target_link_libraries(umicom-ai-coding-runtime-response-modify-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-coding-runtime-response-modify-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-coding-runtime-response-modify-test)
    endif()
    add_test(NAME framework.ai_coding_runtime.response-modify COMMAND umicom-ai-coding-runtime-response-modify-test)

    add_executable(
        umicom-ai-coding-runtime-selection-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime/test_selection.c"
    )
    target_include_directories(
        umicom-ai-coding-runtime-selection-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime"
    )
    target_link_libraries(umicom-ai-coding-runtime-selection-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-coding-runtime-selection-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-coding-runtime-selection-test)
    endif()
    add_test(NAME framework.ai_coding_runtime.selection COMMAND umicom-ai-coding-runtime-selection-test)

    add_executable(
        umicom-ai-coding-runtime-session-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime/test_session.c"
    )
    target_include_directories(
        umicom-ai-coding-runtime-session-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime"
    )
    target_link_libraries(umicom-ai-coding-runtime-session-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-coding-runtime-session-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-coding-runtime-session-test)
    endif()
    add_test(NAME framework.ai_coding_runtime.session COMMAND umicom-ai-coding-runtime-session-test)

    add_executable(
        umicom-ai-coding-runtime-task-queue-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime/test_task_queue.c"
    )
    target_include_directories(
        umicom-ai-coding-runtime-task-queue-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime"
    )
    target_link_libraries(umicom-ai-coding-runtime-task-queue-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-coding-runtime-task-queue-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-coding-runtime-task-queue-test)
    endif()
    add_test(NAME framework.ai_coding_runtime.task-queue COMMAND umicom-ai-coding-runtime-task-queue-test)

    add_executable(
        umicom-ai-coding-runtime-types-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime/test_types.c"
    )
    target_include_directories(
        umicom-ai-coding-runtime-types-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime"
    )
    target_link_libraries(umicom-ai-coding-runtime-types-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-coding-runtime-types-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-coding-runtime-types-test)
    endif()
    add_test(NAME framework.ai_coding_runtime.types COMMAND umicom-ai-coding-runtime-types-test)

    add_executable(
        umicom-ai-coding-runtime-validation-feedback-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime/test_validation_feedback.c"
    )
    target_include_directories(
        umicom-ai-coding-runtime-validation-feedback-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime"
    )
    target_link_libraries(umicom-ai-coding-runtime-validation-feedback-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-coding-runtime-validation-feedback-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-coding-runtime-validation-feedback-test)
    endif()
    add_test(NAME framework.ai_coding_runtime.validation-feedback COMMAND umicom-ai-coding-runtime-validation-feedback-test)

    add_executable(
        umicom-ai-coding-runtime-validation-plan-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime/test_validation_plan.c"
    )
    target_include_directories(
        umicom-ai-coding-runtime-validation-plan-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime"
    )
    target_link_libraries(umicom-ai-coding-runtime-validation-plan-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-coding-runtime-validation-plan-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-coding-runtime-validation-plan-test)
    endif()
    add_test(NAME framework.ai_coding_runtime.validation-plan COMMAND umicom-ai-coding-runtime-validation-plan-test)

    add_executable(
        umicom-ai-coding-runtime-validation-profile-cargo-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime/test_validation_profile_cargo.c"
    )
    target_include_directories(
        umicom-ai-coding-runtime-validation-profile-cargo-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime"
    )
    target_link_libraries(umicom-ai-coding-runtime-validation-profile-cargo-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-coding-runtime-validation-profile-cargo-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-coding-runtime-validation-profile-cargo-test)
    endif()
    add_test(NAME framework.ai_coding_runtime.validation-profile-cargo COMMAND umicom-ai-coding-runtime-validation-profile-cargo-test)

    add_executable(
        umicom-ai-coding-runtime-validation-profile-cmake-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime/test_validation_profile_cmake.c"
    )
    target_include_directories(
        umicom-ai-coding-runtime-validation-profile-cmake-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime"
    )
    target_link_libraries(umicom-ai-coding-runtime-validation-profile-cmake-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-coding-runtime-validation-profile-cmake-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-coding-runtime-validation-profile-cmake-test)
    endif()
    add_test(NAME framework.ai_coding_runtime.validation-profile-cmake COMMAND umicom-ai-coding-runtime-validation-profile-cmake-test)

    add_executable(
        umicom-ai-coding-runtime-validation-profile-go-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime/test_validation_profile_go.c"
    )
    target_include_directories(
        umicom-ai-coding-runtime-validation-profile-go-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime"
    )
    target_link_libraries(umicom-ai-coding-runtime-validation-profile-go-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-coding-runtime-validation-profile-go-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-coding-runtime-validation-profile-go-test)
    endif()
    add_test(NAME framework.ai_coding_runtime.validation-profile-go COMMAND umicom-ai-coding-runtime-validation-profile-go-test)

    add_executable(
        umicom-ai-coding-runtime-validation-profile-gradle-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime/test_validation_profile_gradle.c"
    )
    target_include_directories(
        umicom-ai-coding-runtime-validation-profile-gradle-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime"
    )
    target_link_libraries(umicom-ai-coding-runtime-validation-profile-gradle-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-coding-runtime-validation-profile-gradle-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-coding-runtime-validation-profile-gradle-test)
    endif()
    add_test(NAME framework.ai_coding_runtime.validation-profile-gradle COMMAND umicom-ai-coding-runtime-validation-profile-gradle-test)

    add_executable(
        umicom-ai-coding-runtime-validation-profile-make-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime/test_validation_profile_make.c"
    )
    target_include_directories(
        umicom-ai-coding-runtime-validation-profile-make-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime"
    )
    target_link_libraries(umicom-ai-coding-runtime-validation-profile-make-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-coding-runtime-validation-profile-make-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-coding-runtime-validation-profile-make-test)
    endif()
    add_test(NAME framework.ai_coding_runtime.validation-profile-make COMMAND umicom-ai-coding-runtime-validation-profile-make-test)

    add_executable(
        umicom-ai-coding-runtime-validation-profile-maven-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime/test_validation_profile_maven.c"
    )
    target_include_directories(
        umicom-ai-coding-runtime-validation-profile-maven-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime"
    )
    target_link_libraries(umicom-ai-coding-runtime-validation-profile-maven-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-coding-runtime-validation-profile-maven-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-coding-runtime-validation-profile-maven-test)
    endif()
    add_test(NAME framework.ai_coding_runtime.validation-profile-maven COMMAND umicom-ai-coding-runtime-validation-profile-maven-test)

    add_executable(
        umicom-ai-coding-runtime-validation-profile-meson-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime/test_validation_profile_meson.c"
    )
    target_include_directories(
        umicom-ai-coding-runtime-validation-profile-meson-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime"
    )
    target_link_libraries(umicom-ai-coding-runtime-validation-profile-meson-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-coding-runtime-validation-profile-meson-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-coding-runtime-validation-profile-meson-test)
    endif()
    add_test(NAME framework.ai_coding_runtime.validation-profile-meson COMMAND umicom-ai-coding-runtime-validation-profile-meson-test)

    add_executable(
        umicom-ai-coding-runtime-validation-profile-npm-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime/test_validation_profile_npm.c"
    )
    target_include_directories(
        umicom-ai-coding-runtime-validation-profile-npm-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime"
    )
    target_link_libraries(umicom-ai-coding-runtime-validation-profile-npm-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-coding-runtime-validation-profile-npm-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-coding-runtime-validation-profile-npm-test)
    endif()
    add_test(NAME framework.ai_coding_runtime.validation-profile-npm COMMAND umicom-ai-coding-runtime-validation-profile-npm-test)

    add_executable(
        umicom-ai-coding-runtime-validation-profile-python-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime/test_validation_profile_python.c"
    )
    target_include_directories(
        umicom-ai-coding-runtime-validation-profile-python-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime"
    )
    target_link_libraries(umicom-ai-coding-runtime-validation-profile-python-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-coding-runtime-validation-profile-python-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-coding-runtime-validation-profile-python-test)
    endif()
    add_test(NAME framework.ai_coding_runtime.validation-profile-python COMMAND umicom-ai-coding-runtime-validation-profile-python-test)

    add_executable(
        umicom-ai-coding-runtime-validation-profile-zig-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime/test_validation_profile_zig.c"
    )
    target_include_directories(
        umicom-ai-coding-runtime-validation-profile-zig-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime"
    )
    target_link_libraries(umicom-ai-coding-runtime-validation-profile-zig-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-coding-runtime-validation-profile-zig-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-coding-runtime-validation-profile-zig-test)
    endif()
    add_test(NAME framework.ai_coding_runtime.validation-profile-zig COMMAND umicom-ai-coding-runtime-validation-profile-zig-test)

    add_executable(
        umicom-ai-coding-runtime-validation-runner-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime/test_validation_runner.c"
    )
    target_include_directories(
        umicom-ai-coding-runtime-validation-runner-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime"
    )
    target_link_libraries(umicom-ai-coding-runtime-validation-runner-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-coding-runtime-validation-runner-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-coding-runtime-validation-runner-test)
    endif()
    add_test(NAME framework.ai_coding_runtime.validation-runner COMMAND umicom-ai-coding-runtime-validation-runner-test)

    add_executable(
        umicom-ai-coding-runtime-validation-selector-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime/test_validation_selector.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime/workspace_test_support.c"
    )
    target_include_directories(
        umicom-ai-coding-runtime-validation-selector-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime"
    )
    target_link_libraries(umicom-ai-coding-runtime-validation-selector-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-coding-runtime-validation-selector-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-coding-runtime-validation-selector-test)
    endif()
    add_test(NAME framework.ai_coding_runtime.validation-selector COMMAND umicom-ai-coding-runtime-validation-selector-test)

    add_executable(
        umicom-ai-coding-runtime-workspace-adapter-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime/test_workspace_adapter.c"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime/workspace_test_support.c"
    )
    target_include_directories(
        umicom-ai-coding-runtime-workspace-adapter-test
        PRIVATE
            "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_coding_runtime"
    )
    target_link_libraries(umicom-ai-coding-runtime-workspace-adapter-test PRIVATE Umicom::Framework)
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-coding-runtime-workspace-adapter-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-coding-runtime-workspace-adapter-test)
    endif()
    add_test(NAME framework.ai_coding_runtime.workspace-adapter COMMAND umicom-ai-coding-runtime-workspace-adapter-test)

    add_executable(
        umicom-devprod-problem-navigation-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/developer_productivity/test_workbench_problem_navigation_contract.c"
    )
    set_target_properties(
        umicom-devprod-problem-navigation-test
        PROPERTIES OUTPUT_NAME
                   umicom-developer-productivity-workbench-problem-navigation-contract-test
    )
    add_custom_target(
        umicom-developer-productivity-workbench-problem-navigation-contract-test
        DEPENDS umicom-devprod-problem-navigation-test
    )
    target_link_libraries(
        umicom-devprod-problem-navigation-test
        PRIVATE Umicom::Framework
    )
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(
            umicom-devprod-problem-navigation-test)
    endif()
    # Use the shared build helper when it is available from the parent composition.
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(
            umicom-devprod-problem-navigation-test)
    endif()
    add_test(
        NAME framework.developer_productivity.workbench-problem-navigation-contract
        COMMAND umicom-devprod-problem-navigation-test
    )

endif()

message(STATUS
    "Umicom operational AI coding and vibe-coding runtime enabled")
