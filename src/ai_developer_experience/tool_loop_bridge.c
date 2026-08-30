/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_experience/tool_loop_bridge.c
 *
 * PURPOSE:
 *   Connect coding-tool approval stops to the reusable approval service.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_developer_experience/tool_loop_bridge.h"

UmiStatus umi_ai_developer_tool_loop_configure(
    UmiAiDeveloperExperiencePlatform *platform,
    UmiAiCodingToolLoopConfig *config)
{
    UmiAiDeveloperApprovalService *approvals;

    if (platform == NULL || config == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    approvals = umi_ai_developer_experience_platform_approvals(platform);
    if (approvals == NULL) return UMI_STATUS_INVALID_STATE;

    config->approval = umi_ai_developer_tool_approval_callback;
    config->approval_user_data = approvals;
    return UMI_STATUS_OK;
}
