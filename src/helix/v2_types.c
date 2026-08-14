/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/helix/v2_types.c
 * PURPOSE: Implement stable Helix Agent Runtime v2 text and transition rules.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/helix/v2_types.h"

const char *umi_helix_agent_role_v2_text(UmiHelixAgentRoleV2 role)
{
    switch (role) {
        case UMI_HELIX_AGENT_ROLE_DESIGNER: return "designer";
        case UMI_HELIX_AGENT_ROLE_BUILDER: return "builder";
        case UMI_HELIX_AGENT_ROLE_SUGGESTION: return "suggestion";
        case UMI_HELIX_AGENT_ROLE_REVIEWER: return "reviewer";
        case UMI_HELIX_AGENT_ROLE_TEST: return "test";
        default: return "unknown";
    }
}

const char *umi_helix_action_kind_v2_text(UmiHelixActionKindV2 kind)
{
    switch (kind) {
        case UMI_HELIX_ACTION_READ: return "read";
        case UMI_HELIX_ACTION_FILESYSTEM: return "filesystem";
        case UMI_HELIX_ACTION_BUILD: return "build";
        case UMI_HELIX_ACTION_TEST: return "test";
        case UMI_HELIX_ACTION_REVIEW: return "review";
        case UMI_HELIX_ACTION_SOURCE_CONTROL: return "source-control";
        default: return "unknown";
    }
}

const char *umi_helix_operation_state_v2_text(UmiHelixOperationStateV2 state)
{
    switch (state) {
        case UMI_HELIX_OPERATION_DRAFT: return "draft";
        case UMI_HELIX_OPERATION_PLANNED: return "planned";
        case UMI_HELIX_OPERATION_AWAITING_APPROVAL: return "awaiting-approval";
        case UMI_HELIX_OPERATION_APPROVED: return "approved";
        case UMI_HELIX_OPERATION_RUNNING: return "running";
        case UMI_HELIX_OPERATION_SUCCEEDED: return "succeeded";
        case UMI_HELIX_OPERATION_FAILED: return "failed";
        case UMI_HELIX_OPERATION_ROLLING_BACK: return "rolling-back";
        case UMI_HELIX_OPERATION_ROLLED_BACK: return "rolled-back";
        case UMI_HELIX_OPERATION_CANCELLED: return "cancelled";
        default: return "unknown";
    }
}

int umi_helix_action_kind_v2_is_mutating(UmiHelixActionKindV2 kind)
{
    return kind == UMI_HELIX_ACTION_FILESYSTEM ||
           kind == UMI_HELIX_ACTION_BUILD ||
           kind == UMI_HELIX_ACTION_SOURCE_CONTROL;
}

int umi_helix_operation_state_v2_can_transition(
    UmiHelixOperationStateV2 current,
    UmiHelixOperationStateV2 next)
{
    if (current == next) return 1;
    if (next == UMI_HELIX_OPERATION_CANCELLED) {
        return current != UMI_HELIX_OPERATION_SUCCEEDED &&
               current != UMI_HELIX_OPERATION_ROLLED_BACK;
    }
    switch (current) {
        case UMI_HELIX_OPERATION_DRAFT:
            return next == UMI_HELIX_OPERATION_PLANNED;
        case UMI_HELIX_OPERATION_PLANNED:
            return next == UMI_HELIX_OPERATION_AWAITING_APPROVAL ||
                   next == UMI_HELIX_OPERATION_APPROVED;
        case UMI_HELIX_OPERATION_AWAITING_APPROVAL:
            return next == UMI_HELIX_OPERATION_APPROVED;
        case UMI_HELIX_OPERATION_APPROVED:
            return next == UMI_HELIX_OPERATION_RUNNING;
        case UMI_HELIX_OPERATION_RUNNING:
            return next == UMI_HELIX_OPERATION_SUCCEEDED ||
                   next == UMI_HELIX_OPERATION_FAILED ||
                   next == UMI_HELIX_OPERATION_ROLLING_BACK;
        case UMI_HELIX_OPERATION_FAILED:
            return next == UMI_HELIX_OPERATION_APPROVED ||
                   next == UMI_HELIX_OPERATION_ROLLING_BACK;
        case UMI_HELIX_OPERATION_SUCCEEDED:
            return next == UMI_HELIX_OPERATION_ROLLING_BACK;
        case UMI_HELIX_OPERATION_ROLLING_BACK:
            return next == UMI_HELIX_OPERATION_ROLLED_BACK ||
                   next == UMI_HELIX_OPERATION_FAILED;
        default:
            return 0;
    }
}
