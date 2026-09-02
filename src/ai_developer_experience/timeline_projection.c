/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_experience/timeline_projection.c
 *
 * PURPOSE:
 *   Implement cross-service timeline projections.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_developer_experience/timeline_projection.h"

#include <stdio.h>
#include <string.h>

/* Provide the copy text operation used by this module and its client applications. */
static UmiStatus copy_text(char *out, size_t capacity, const char *text)
{
    size_t length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out == NULL || capacity == 0U || text == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    length = strlen(text);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length >= capacity) length = capacity - 1U;

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (length > 0U) (void)memcpy(out, text, length);
    out[length] = '\0';
    return UMI_STATUS_OK;
}

/*
 * Provide the ai developer timeline from agent event operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_developer_timeline_from_agent_event(
    const UmiAiCodingEvent *event,
    UmiAiDeveloperTimelineEvent *out_event)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (event == NULL || out_event == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_event, 0, sizeof(*out_event));
    out_event->sequence = event->sequence;
    out_event->status = event->status;
    out_event->important =
        event->kind == UMI_AI_CODING_EVENT_PATCH ||
        event->kind == UMI_AI_CODING_EVENT_VALIDATION ||
        event->state == UMI_AI_CODING_RUNTIME_FAILED;

    (void)copy_text(
        out_event->task_id,
        sizeof(out_event->task_id),
        event->task_id);
    (void)copy_text(
        out_event->category,
        sizeof(out_event->category),
        umi_ai_coding_event_kind_text(event->kind));
    (void)snprintf(
        out_event->title,
        sizeof(out_event->title),
        "Agent %s",
        umi_ai_coding_runtime_state_text(event->state));
    (void)copy_text(
        out_event->detail,
        sizeof(out_event->detail),
        event->message);

    return UMI_STATUS_OK;
}

/*
 * Provide the ai developer timeline from tool result operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_developer_timeline_from_tool_result(
    const char *task_id,
    const UmiAiCodingToolResult *result,
    UmiAiDeveloperTimelineEvent *out_event)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (task_id == NULL || result == NULL || out_event == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_event, 0, sizeof(*out_event));
    out_event->sequence = result->sequence;
    out_event->status = result->status;
    out_event->important =
        result->state != UMI_AI_CODING_TOOL_CALL_SUCCEEDED;

    (void)copy_text(
        out_event->task_id,
        sizeof(out_event->task_id),
        task_id);
    (void)copy_text(
        out_event->category,
        sizeof(out_event->category),
        "tool");
    (void)snprintf(
        out_event->title,
        sizeof(out_event->title),
        "%s: %s",
        result->tool_id,
        umi_ai_coding_tool_call_state_text(result->state));
    (void)copy_text(
        out_event->detail,
        sizeof(out_event->detail),
        result->output);

    return UMI_STATUS_OK;
}

/*
 * Provide the ai developer timeline from validation operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_developer_timeline_from_validation(
    const char *task_id,
    const UmiAiCodingValidationResult *result,
    uint64_t sequence,
    UmiAiDeveloperTimelineEvent *out_event)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (task_id == NULL || result == NULL || out_event == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_event, 0, sizeof(*out_event));
    out_event->sequence = sequence;
    out_event->status = result->status;
    out_event->important = !result->passed;

    (void)copy_text(
        out_event->task_id,
        sizeof(out_event->task_id),
        task_id);
    (void)copy_text(
        out_event->category,
        sizeof(out_event->category),
        "validation");
    (void)snprintf(
        out_event->title,
        sizeof(out_event->title),
        "%s: %s",
        result->step_id,
        result->passed ? "passed" : "failed");
    (void)copy_text(
        out_event->detail,
        sizeof(out_event->detail),
        result->output);

    return UMI_STATUS_OK;
}

/*
 * Provide the ai developer timeline from approval operation used by this module and its
 * client applications.
 */
UmiStatus umi_ai_developer_timeline_from_approval(
    const UmiAiDeveloperApprovalRequest *approval,
    UmiAiDeveloperTimelineEvent *out_event)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (approval == NULL || out_event == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_event, 0, sizeof(*out_event));
    out_event->sequence =
        approval->decided_sequence != 0U
            ? approval->decided_sequence
            : approval->requested_sequence;
    out_event->status =
        approval->state == UMI_AI_DEVELOPER_APPROVAL_REJECTED
            ? UMI_STATUS_PERMISSION_DENIED
            : UMI_STATUS_OK;
    out_event->important = 1;

    (void)copy_text(
        out_event->category,
        sizeof(out_event->category),
        "approval");
    (void)snprintf(
        out_event->title,
        sizeof(out_event->title),
        "%.190s: %.48s",
        approval->title,
        umi_ai_developer_approval_state_text(approval->state));
    (void)copy_text(
        out_event->detail,
        sizeof(out_event->detail),
        approval->summary);

    return UMI_STATUS_OK;
}
