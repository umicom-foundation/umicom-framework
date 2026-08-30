#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomAiDeveloperExperiencePlatform.cmake
#
# PURPOSE:
#   Attach the toolkit-neutral AI Developer Experience platform to the canonical
#   developer library and register focused regression coverage.
#
# ARCHITECTURE:
#   Operational coding/runtime/tools remain authoritative. This integration adds
#   approvals, line/split review, task/timeline projection, durable Session Store
#   state, command registry integration, AI Developer perspective and reusable
#   view models for thin applications such as Umicom Studio.
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
        "AI Developer Experience requires the canonical umicom_developer target")
endif()

target_sources(umicom_developer PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_developer_experience/action_ids.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_developer_experience/activity_sync.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_developer_experience/approval_persistence.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_developer_experience/approval_queue.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_developer_experience/approval_service.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_developer_experience/chat_controller.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_developer_experience/chat_persistence.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_developer_experience/command.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_developer_experience/command_context.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_developer_experience/command_registry_bridge.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_developer_experience/command_router.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_developer_experience/diff_builder.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_developer_experience/diff_hunks.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_developer_experience/diff_navigation.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_developer_experience/durable_store.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_developer_experience/patch_review.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_developer_experience/patch_review_service.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_developer_experience/persistence_codec.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_developer_experience/perspective.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_developer_experience/platform.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_developer_experience/preferences.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_developer_experience/preferences_persistence.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_developer_experience/presentation_persistence.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_developer_experience/presentation_state.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_developer_experience/restore_report.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_developer_experience/session_bundle.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_developer_experience/side_by_side.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_developer_experience/state_migration.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_developer_experience/state_schema.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_developer_experience/task_controller.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_developer_experience/task_persistence.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_developer_experience/task_projection.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_developer_experience/task_registry.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_developer_experience/timeline.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_developer_experience/timeline_projection.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_developer_experience/tool_loop_bridge.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_developer_experience/types.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_developer_experience/view_factory.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_developer_experience/view_helpers.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_developer_experience/view_ids.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_developer_experience/views/approvals.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_developer_experience/views/chat.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_developer_experience/views/checkpoints.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_developer_experience/views/context.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_developer_experience/views/diff.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_developer_experience/views/history.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_developer_experience/views/overview.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_developer_experience/views/patch_review.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_developer_experience/views/policy.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_developer_experience/views/tasks.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_developer_experience/views/tool_activity.c"
    "${CMAKE_CURRENT_LIST_DIR}/../src/ai_developer_experience/views/validation.c"
)

target_link_libraries(umicom_developer PUBLIC
    Umicom::platform
    Umicom::runtime
)

if(BUILD_TESTING)
    add_executable(
        umicom-ai-developer-experience-activity-sync-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_developer_experience/test_activity_sync.c"
    )
    target_link_libraries(umicom-ai-developer-experience-activity-sync-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-developer-experience-activity-sync-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-developer-experience-activity-sync-test)
    endif()
    add_test(NAME framework.ai_developer_experience.activity-sync COMMAND umicom-ai-developer-experience-activity-sync-test)

    add_executable(
        umicom-ai-developer-experience-approval-persistence-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_developer_experience/test_approval_persistence.c"
    )
    target_link_libraries(umicom-ai-developer-experience-approval-persistence-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-developer-experience-approval-persistence-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-developer-experience-approval-persistence-test)
    endif()
    add_test(NAME framework.ai_developer_experience.approval-persistence COMMAND umicom-ai-developer-experience-approval-persistence-test)

    add_executable(
        umicom-ai-developer-experience-approval-queue-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_developer_experience/test_approval_queue.c"
    )
    target_link_libraries(umicom-ai-developer-experience-approval-queue-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-developer-experience-approval-queue-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-developer-experience-approval-queue-test)
    endif()
    add_test(NAME framework.ai_developer_experience.approval-queue COMMAND umicom-ai-developer-experience-approval-queue-test)

    add_executable(
        umicom-ai-developer-experience-approval-service-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_developer_experience/test_approval_service.c"
    )
    target_link_libraries(umicom-ai-developer-experience-approval-service-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-developer-experience-approval-service-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-developer-experience-approval-service-test)
    endif()
    add_test(NAME framework.ai_developer_experience.approval-service COMMAND umicom-ai-developer-experience-approval-service-test)

    add_executable(
        umicom-ai-developer-experience-approvals-view-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_developer_experience/test_approvals_view.c"
    )
    target_link_libraries(umicom-ai-developer-experience-approvals-view-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-developer-experience-approvals-view-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-developer-experience-approvals-view-test)
    endif()
    add_test(NAME framework.ai_developer_experience.approvals-view COMMAND umicom-ai-developer-experience-approvals-view-test)

    add_executable(
        umicom-ai-developer-experience-chat-persistence-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_developer_experience/test_chat_persistence.c"
    )
    target_link_libraries(umicom-ai-developer-experience-chat-persistence-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-developer-experience-chat-persistence-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-developer-experience-chat-persistence-test)
    endif()
    add_test(NAME framework.ai_developer_experience.chat-persistence COMMAND umicom-ai-developer-experience-chat-persistence-test)

    add_executable(
        umicom-ai-developer-experience-command-catalogue-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_developer_experience/test_command_catalogue.c"
    )
    target_link_libraries(umicom-ai-developer-experience-command-catalogue-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-developer-experience-command-catalogue-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-developer-experience-command-catalogue-test)
    endif()
    add_test(NAME framework.ai_developer_experience.command-catalogue COMMAND umicom-ai-developer-experience-command-catalogue-test)

    add_executable(
        umicom-ai-developer-experience-command-context-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_developer_experience/test_command_context.c"
    )
    target_link_libraries(umicom-ai-developer-experience-command-context-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-developer-experience-command-context-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-developer-experience-command-context-test)
    endif()
    add_test(NAME framework.ai_developer_experience.command-context COMMAND umicom-ai-developer-experience-command-context-test)

    add_executable(
        umicom-ai-developer-experience-diff-builder-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_developer_experience/test_diff_builder.c"
    )
    target_link_libraries(umicom-ai-developer-experience-diff-builder-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-developer-experience-diff-builder-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-developer-experience-diff-builder-test)
    endif()
    add_test(NAME framework.ai_developer_experience.diff-builder COMMAND umicom-ai-developer-experience-diff-builder-test)

    add_executable(
        umicom-ai-developer-experience-diff-create-delete-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_developer_experience/test_diff_create_delete.c"
    )
    target_link_libraries(umicom-ai-developer-experience-diff-create-delete-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-developer-experience-diff-create-delete-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-developer-experience-diff-create-delete-test)
    endif()
    add_test(NAME framework.ai_developer_experience.diff-create-delete COMMAND umicom-ai-developer-experience-diff-create-delete-test)

    add_executable(
        umicom-ai-developer-experience-diff-hunks-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_developer_experience/test_diff_hunks.c"
    )
    target_link_libraries(umicom-ai-developer-experience-diff-hunks-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-developer-experience-diff-hunks-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-developer-experience-diff-hunks-test)
    endif()
    add_test(NAME framework.ai_developer_experience.diff-hunks COMMAND umicom-ai-developer-experience-diff-hunks-test)

    add_executable(
        umicom-ai-developer-experience-diff-navigation-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_developer_experience/test_diff_navigation.c"
    )
    target_link_libraries(umicom-ai-developer-experience-diff-navigation-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-developer-experience-diff-navigation-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-developer-experience-diff-navigation-test)
    endif()
    add_test(NAME framework.ai_developer_experience.diff-navigation COMMAND umicom-ai-developer-experience-diff-navigation-test)

    add_executable(
        umicom-ai-developer-experience-diff-view-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_developer_experience/test_diff_view.c"
    )
    target_link_libraries(umicom-ai-developer-experience-diff-view-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-developer-experience-diff-view-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-developer-experience-diff-view-test)
    endif()
    add_test(NAME framework.ai_developer_experience.diff-view COMMAND umicom-ai-developer-experience-diff-view-test)

    add_executable(
        umicom-ai-developer-experience-history-view-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_developer_experience/test_history_view.c"
    )
    target_link_libraries(umicom-ai-developer-experience-history-view-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-developer-experience-history-view-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-developer-experience-history-view-test)
    endif()
    add_test(NAME framework.ai_developer_experience.history-view COMMAND umicom-ai-developer-experience-history-view-test)

    add_executable(
        umicom-ai-developer-experience-ids-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_developer_experience/test_ids.c"
    )
    target_link_libraries(umicom-ai-developer-experience-ids-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-developer-experience-ids-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-developer-experience-ids-test)
    endif()
    add_test(NAME framework.ai_developer_experience.ids COMMAND umicom-ai-developer-experience-ids-test)

    add_executable(
        umicom-ai-developer-experience-patch-review-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_developer_experience/test_patch_review.c"
    )
    target_link_libraries(umicom-ai-developer-experience-patch-review-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-developer-experience-patch-review-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-developer-experience-patch-review-test)
    endif()
    add_test(NAME framework.ai_developer_experience.patch-review COMMAND umicom-ai-developer-experience-patch-review-test)

    add_executable(
        umicom-ai-developer-experience-patch-review-service-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_developer_experience/test_patch_review_service.c"
    )
    target_link_libraries(umicom-ai-developer-experience-patch-review-service-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-developer-experience-patch-review-service-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-developer-experience-patch-review-service-test)
    endif()
    add_test(NAME framework.ai_developer_experience.patch-review-service COMMAND umicom-ai-developer-experience-patch-review-service-test)

    add_executable(
        umicom-ai-developer-experience-patch-review-view-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_developer_experience/test_patch_review_view.c"
    )
    target_link_libraries(umicom-ai-developer-experience-patch-review-view-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-developer-experience-patch-review-view-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-developer-experience-patch-review-view-test)
    endif()
    add_test(NAME framework.ai_developer_experience.patch-review-view COMMAND umicom-ai-developer-experience-patch-review-view-test)

    add_executable(
        umicom-ai-developer-experience-persistence-budget-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_developer_experience/test_persistence_budget.c"
    )
    target_link_libraries(umicom-ai-developer-experience-persistence-budget-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-developer-experience-persistence-budget-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-developer-experience-persistence-budget-test)
    endif()
    add_test(NAME framework.ai_developer_experience.persistence-budget COMMAND umicom-ai-developer-experience-persistence-budget-test)

    add_executable(
        umicom-ai-developer-experience-persistence-codec-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_developer_experience/test_persistence_codec.c"
    )
    target_link_libraries(umicom-ai-developer-experience-persistence-codec-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-developer-experience-persistence-codec-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-developer-experience-persistence-codec-test)
    endif()
    add_test(NAME framework.ai_developer_experience.persistence-codec COMMAND umicom-ai-developer-experience-persistence-codec-test)

    add_executable(
        umicom-ai-developer-experience-perspective-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_developer_experience/test_perspective.c"
    )
    target_link_libraries(umicom-ai-developer-experience-perspective-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-developer-experience-perspective-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-developer-experience-perspective-test)
    endif()
    add_test(NAME framework.ai_developer_experience.perspective COMMAND umicom-ai-developer-experience-perspective-test)

    add_executable(
        umicom-ai-developer-experience-preferences-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_developer_experience/test_preferences.c"
    )
    target_link_libraries(umicom-ai-developer-experience-preferences-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-developer-experience-preferences-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-developer-experience-preferences-test)
    endif()
    add_test(NAME framework.ai_developer_experience.preferences COMMAND umicom-ai-developer-experience-preferences-test)

    add_executable(
        umicom-ai-developer-experience-preferences-persistence-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_developer_experience/test_preferences_persistence.c"
    )
    target_link_libraries(umicom-ai-developer-experience-preferences-persistence-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-developer-experience-preferences-persistence-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-developer-experience-preferences-persistence-test)
    endif()
    add_test(NAME framework.ai_developer_experience.preferences-persistence COMMAND umicom-ai-developer-experience-preferences-persistence-test)

    add_executable(
        umicom-ai-developer-experience-presentation-persistence-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_developer_experience/test_presentation_persistence.c"
    )
    target_link_libraries(umicom-ai-developer-experience-presentation-persistence-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-developer-experience-presentation-persistence-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-developer-experience-presentation-persistence-test)
    endif()
    add_test(NAME framework.ai_developer_experience.presentation-persistence COMMAND umicom-ai-developer-experience-presentation-persistence-test)

    add_executable(
        umicom-ai-developer-experience-presentation-state-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_developer_experience/test_presentation_state.c"
    )
    target_link_libraries(umicom-ai-developer-experience-presentation-state-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-developer-experience-presentation-state-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-developer-experience-presentation-state-test)
    endif()
    add_test(NAME framework.ai_developer_experience.presentation-state COMMAND umicom-ai-developer-experience-presentation-state-test)

    add_executable(
        umicom-ai-developer-experience-session-bundle-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_developer_experience/test_session_bundle.c"
    )
    target_link_libraries(umicom-ai-developer-experience-session-bundle-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-developer-experience-session-bundle-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-developer-experience-session-bundle-test)
    endif()
    add_test(NAME framework.ai_developer_experience.session-bundle COMMAND umicom-ai-developer-experience-session-bundle-test)

    add_executable(
        umicom-ai-developer-experience-side-by-side-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_developer_experience/test_side_by_side.c"
    )
    target_link_libraries(umicom-ai-developer-experience-side-by-side-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-developer-experience-side-by-side-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-developer-experience-side-by-side-test)
    endif()
    add_test(NAME framework.ai_developer_experience.side-by-side COMMAND umicom-ai-developer-experience-side-by-side-test)

    add_executable(
        umicom-ai-developer-experience-state-schema-migration-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_developer_experience/test_state_schema_migration.c"
    )
    target_link_libraries(umicom-ai-developer-experience-state-schema-migration-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-developer-experience-state-schema-migration-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-developer-experience-state-schema-migration-test)
    endif()
    add_test(NAME framework.ai_developer_experience.state-schema-migration COMMAND umicom-ai-developer-experience-state-schema-migration-test)

    add_executable(
        umicom-ai-developer-experience-task-persistence-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_developer_experience/test_task_persistence.c"
    )
    target_link_libraries(umicom-ai-developer-experience-task-persistence-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-developer-experience-task-persistence-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-developer-experience-task-persistence-test)
    endif()
    add_test(NAME framework.ai_developer_experience.task-persistence COMMAND umicom-ai-developer-experience-task-persistence-test)

    add_executable(
        umicom-ai-developer-experience-task-projection-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_developer_experience/test_task_projection.c"
    )
    target_link_libraries(umicom-ai-developer-experience-task-projection-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-developer-experience-task-projection-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-developer-experience-task-projection-test)
    endif()
    add_test(NAME framework.ai_developer_experience.task-projection COMMAND umicom-ai-developer-experience-task-projection-test)

    add_executable(
        umicom-ai-developer-experience-task-registry-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_developer_experience/test_task_registry.c"
    )
    target_link_libraries(umicom-ai-developer-experience-task-registry-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-developer-experience-task-registry-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-developer-experience-task-registry-test)
    endif()
    add_test(NAME framework.ai_developer_experience.task-registry COMMAND umicom-ai-developer-experience-task-registry-test)

    add_executable(
        umicom-ai-developer-experience-tasks-view-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_developer_experience/test_tasks_view.c"
    )
    target_link_libraries(umicom-ai-developer-experience-tasks-view-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-developer-experience-tasks-view-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-developer-experience-tasks-view-test)
    endif()
    add_test(NAME framework.ai_developer_experience.tasks-view COMMAND umicom-ai-developer-experience-tasks-view-test)

    add_executable(
        umicom-ai-developer-experience-timeline-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_developer_experience/test_timeline.c"
    )
    target_link_libraries(umicom-ai-developer-experience-timeline-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-developer-experience-timeline-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-developer-experience-timeline-test)
    endif()
    add_test(NAME framework.ai_developer_experience.timeline COMMAND umicom-ai-developer-experience-timeline-test)

    add_executable(
        umicom-ai-developer-experience-timeline-projection-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_developer_experience/test_timeline_projection.c"
    )
    target_link_libraries(umicom-ai-developer-experience-timeline-projection-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-developer-experience-timeline-projection-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-developer-experience-timeline-projection-test)
    endif()
    add_test(NAME framework.ai_developer_experience.timeline-projection COMMAND umicom-ai-developer-experience-timeline-projection-test)

    add_executable(
        umicom-ai-developer-experience-types-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/ai_developer_experience/test_types.c"
    )
    target_link_libraries(umicom-ai-developer-experience-types-test PRIVATE Umicom::Framework)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(umicom-ai-developer-experience-types-test)
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-ai-developer-experience-types-test)
    endif()
    add_test(NAME framework.ai_developer_experience.types COMMAND umicom-ai-developer-experience-types-test)

endif()

message(STATUS
    "Umicom AI Developer Experience review, approvals, durable state and views enabled")
