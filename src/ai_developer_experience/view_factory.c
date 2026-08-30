/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_experience/view_factory.c
 *
 * PURPOSE:
 *   Dispatch AI Developer pane creation from current Framework state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_developer_experience/view_factory.h"

#include "umicom/ai_developer_experience/view_ids.h"

static UmiAiCodingToolChatSession *active_chat(
    UmiAiDeveloperExperiencePlatform *platform)
{
    UmiAiCodingToolsPlatform *tools =
        umi_ai_developer_experience_platform_tools(platform);
    UmiAiDeveloperPresentationState *presentation =
        umi_ai_developer_experience_platform_presentation(platform);
    UmiAiCodingToolChatRegistry *chats;

    if (tools == NULL || presentation == NULL) return NULL;

    chats = umi_ai_coding_tools_platform_chats(tools);
    if (chats == NULL) return NULL;

    if (presentation->active_chat_id[0] != '\0') {
        UmiAiCodingToolChatSession *session =
            umi_ai_coding_tool_chat_registry_find(
                chats,
                presentation->active_chat_id);

        if (session != NULL) return session;
    }

    if (chats->count > 0U) {
        return &chats->sessions[chats->count - 1U];
    }

    return NULL;
}

UmiStatus umi_ai_developer_view_factory_create(
    UmiAiDeveloperExperiencePlatform *platform,
    UmiAiDeveloperPaneKind pane,
    UmiUiViewModel **out_view)
{
    UmiAiDeveloperExperienceSnapshot snapshot;
    UmiAiDeveloperPreferences *preferences;
    UmiAiDeveloperPresentationState *presentation;
    UmiAiCodingToolsPlatform *tools;
    UmiAiCodingRuntimePlatform *coding;
    UmiStatus status;

    if (platform == NULL || out_view == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    *out_view = NULL;

    status = umi_ai_developer_experience_platform_snapshot(
        platform,
        &snapshot);
    if (status != UMI_STATUS_OK) return status;

    preferences =
        umi_ai_developer_experience_platform_preferences(platform);
    presentation =
        umi_ai_developer_experience_platform_presentation(platform);
    tools =
        umi_ai_developer_experience_platform_tools(platform);
    coding =
        umi_ai_developer_experience_platform_coding(platform);

    if (preferences == NULL || presentation == NULL ||
        tools == NULL || coding == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }

    switch (pane) {
        case UMI_AI_DEVELOPER_PANE_OVERVIEW:
            return umi_ai_developer_overview_view_create(
                umi_ai_developer_view_id_overview(),
                &snapshot.coding,
                &snapshot.tools,
                umi_ai_developer_experience_platform_tasks(platform),
                umi_ai_developer_experience_platform_approvals(platform)->queue,
                out_view);

        case UMI_AI_DEVELOPER_PANE_CHAT: {
            UmiAiCodingToolChatSession *session = active_chat(platform);

            return session != NULL
                ? umi_ai_developer_chat_view_create(
                    umi_ai_developer_view_id_chat(),
                    session,
                    preferences->visible_rows,
                    out_view)
                : umi_ai_developer_view_create_base(
                    umi_ai_developer_view_id_chat(),
                    "ai-developer-chat",
                    "AI Coding Chat",
                    "No coding chat session is open.",
                    out_view);
        }

        case UMI_AI_DEVELOPER_PANE_TASKS:
            return umi_ai_developer_tasks_view_create(
                umi_ai_developer_view_id_tasks(),
                umi_ai_developer_experience_platform_tasks(platform),
                presentation->active_task_id,
                preferences->visible_rows,
                out_view);

        case UMI_AI_DEVELOPER_PANE_APPROVALS:
            return umi_ai_developer_approvals_view_create(
                umi_ai_developer_view_id_approvals(),
                umi_ai_developer_experience_platform_approvals(platform)->queue,
                presentation->active_approval_id,
                preferences->visible_rows,
                out_view);

        case UMI_AI_DEVELOPER_PANE_PATCH_REVIEW:
            return umi_ai_developer_patch_review_view_create(
                umi_ai_developer_view_id_patch_review(),
                umi_ai_developer_experience_platform_review(platform),
                out_view);

        case UMI_AI_DEVELOPER_PANE_DIFF:
            return umi_ai_developer_diff_view_create(
                umi_ai_developer_view_id_diff(),
                umi_ai_developer_experience_platform_review(platform),
                preferences,
                presentation->active_diff_line,
                out_view);

        case UMI_AI_DEVELOPER_PANE_TOOL_ACTIVITY:
            return umi_ai_developer_tool_activity_view_create(
                umi_ai_developer_view_id_tool_activity(),
                umi_ai_coding_tools_platform_history(tools),
                preferences->visible_rows,
                out_view);

        case UMI_AI_DEVELOPER_PANE_VALIDATION: {
            UmiAiCodingAgent *agent =
                umi_ai_coding_runtime_platform_agent(coding);
            const UmiAiCodingValidationReport *report =
                umi_ai_coding_agent_last_validation(agent);

            return report != NULL
                ? umi_ai_developer_validation_view_create(
                    umi_ai_developer_view_id_validation(),
                    report,
                    preferences->visible_rows,
                    out_view)
                : UMI_STATUS_INVALID_STATE;
        }

        case UMI_AI_DEVELOPER_PANE_CONTEXT:
            return umi_ai_developer_context_view_create(
                umi_ai_developer_view_id_context(),
                umi_ai_coding_runtime_platform_assistant(coding),
                preferences->visible_rows,
                out_view);

        case UMI_AI_DEVELOPER_PANE_HISTORY:
            return umi_ai_developer_history_view_create(
                umi_ai_developer_view_id_history(),
                umi_ai_developer_experience_platform_timeline(platform),
                preferences->visible_rows,
                out_view);

        case UMI_AI_DEVELOPER_PANE_POLICY:
            return umi_ai_developer_policy_view_create(
                umi_ai_developer_view_id_policy(),
                umi_ai_coding_tools_platform_environment(tools),
                out_view);

        case UMI_AI_DEVELOPER_PANE_CHECKPOINTS:
            return umi_ai_developer_checkpoints_view_create(
                umi_ai_developer_view_id_checkpoints(),
                umi_ai_coding_tools_platform_checkpoints(tools),
                preferences->visible_rows,
                out_view);

        default:
            return UMI_STATUS_INVALID_ARGUMENT;
    }
}
