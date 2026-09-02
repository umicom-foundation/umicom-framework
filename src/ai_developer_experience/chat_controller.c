/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_experience/chat_controller.c
 *
 * PURPOSE:
 *   Implement repository-aware coding chat with approval-aware tool execution.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_developer_experience/chat_controller.h"

/*
 * Provide the ai developer chat open operation used by this module and its client
 * applications.
 */
UmiStatus umi_ai_developer_chat_open(
    UmiAiDeveloperExperiencePlatform *platform,
    const char *session_id,
    const char *provider_id,
    const char *model_id)
{
    UmiAiCodingToolsPlatform *tools;
    UmiAiDeveloperPresentationState *presentation;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL || session_id == NULL ||
        provider_id == NULL || model_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    tools = umi_ai_developer_experience_platform_tools(platform);
    presentation =
        umi_ai_developer_experience_platform_presentation(platform);

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (tools == NULL || presentation == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }

    status = umi_ai_coding_tools_platform_open_chat(
        tools,
        session_id,
        provider_id,
        model_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_ai_developer_presentation_set_chat(
        presentation,
        session_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_ai_developer_experience_platform_select_pane(
            platform,
            UMI_AI_DEVELOPER_PANE_CHAT);
    }

    return status;
}

/*
 * Perform ai developer chat through the module contract so client applications do not
 * duplicate its policy.
 */
UmiStatus umi_ai_developer_chat_run(
    UmiAiDeveloperExperiencePlatform *platform,
    const char *session_id,
    const char *user_message,
    UmiAiCodingToolLoopResult *out_result)
{
    UmiAiCodingToolsPlatform *tools;
    UmiAiCodingToolLoopConfig config;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (platform == NULL || session_id == NULL ||
        user_message == NULL || out_result == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    tools = umi_ai_developer_experience_platform_tools(platform);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (tools == NULL) return UMI_STATUS_INVALID_STATE;

    umi_ai_coding_tool_loop_config_init(&config);

    status = umi_ai_developer_tool_loop_configure(
        platform,
        &config);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    status = umi_ai_coding_tools_platform_run_chat(
        tools,
        session_id,
        &config,
        user_message,
        out_result);

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_PERMISSION_DENIED &&
        out_result->approval_stops > 0U) {
        (void)umi_ai_developer_experience_platform_select_pane(
            platform,
            UMI_AI_DEVELOPER_PANE_APPROVALS);
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        (void)umi_ai_developer_experience_platform_select_pane(
            platform,
            UMI_AI_DEVELOPER_PANE_CHAT);
    }

    (void)umi_ai_developer_experience_platform_refresh(platform);
    return status;
}
