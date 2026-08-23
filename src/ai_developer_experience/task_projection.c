/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_developer_experience/task_projection.c
 *
 * PURPOSE:
 *   Implement coding-agent to developer-task projection.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_developer_experience/task_projection.h"

#include <string.h>

static UmiStatus copy_text(char *out, size_t capacity, const char *text)
{
    size_t length;

    if (out == NULL || capacity == 0U || text == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    length = strlen(text);
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;

    (void)memcpy(out, text, length + 1U);
    return UMI_STATUS_OK;
}

UmiAiDeveloperTaskState umi_ai_developer_task_state_from_agent(
    UmiAiCodingRuntimeState state)
{
    switch (state) {
        case UMI_AI_CODING_RUNTIME_IDLE:
        case UMI_AI_CODING_RUNTIME_PREPARING:
        case UMI_AI_CODING_RUNTIME_MATERIALISING_CONTEXT:
        case UMI_AI_CODING_RUNTIME_GENERATING:
        case UMI_AI_CODING_RUNTIME_APPLYING:
        case UMI_AI_CODING_RUNTIME_REPAIRING:
            return UMI_AI_DEVELOPER_TASK_RUNNING;
        case UMI_AI_CODING_RUNTIME_REVIEW_REQUIRED:
            return UMI_AI_DEVELOPER_TASK_REVIEW;
        case UMI_AI_CODING_RUNTIME_VALIDATING:
            return UMI_AI_DEVELOPER_TASK_VALIDATING;
        case UMI_AI_CODING_RUNTIME_COMPLETED:
            return UMI_AI_DEVELOPER_TASK_COMPLETED;
        case UMI_AI_CODING_RUNTIME_CANCELLED:
            return UMI_AI_DEVELOPER_TASK_CANCELLED;
        case UMI_AI_CODING_RUNTIME_FAILED:
        default:
            return UMI_AI_DEVELOPER_TASK_FAILED;
    }
}

UmiStatus umi_ai_developer_task_project(
    const UmiAiCodingAgentSnapshot *snapshot,
    const char *title,
    const char *summary,
    uint64_t sequence,
    UmiAiDeveloperTaskEntry *out_entry)
{
    UmiStatus status;

    if (snapshot == NULL || title == NULL ||
        summary == NULL || out_entry == NULL ||
        snapshot->task_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_entry, 0, sizeof(*out_entry));

    status = copy_text(
        out_entry->task_id,
        sizeof(out_entry->task_id),
        snapshot->task_id);
    if (status == UMI_STATUS_OK) {
        status = copy_text(
            out_entry->request_id,
            sizeof(out_entry->request_id),
            snapshot->request_id);
    }
    if (status == UMI_STATUS_OK) {
        status = copy_text(
            out_entry->title,
            sizeof(out_entry->title),
            title);
    }
    if (status == UMI_STATUS_OK) {
        status = copy_text(
            out_entry->summary,
            sizeof(out_entry->summary),
            summary);
    }
    if (status != UMI_STATUS_OK) return status;

    out_entry->state =
        umi_ai_developer_task_state_from_agent(snapshot->state);
    out_entry->iteration = snapshot->iteration;
    out_entry->maximum_iterations = snapshot->maximum_iterations;
    out_entry->patch_files = snapshot->patch_files;
    out_entry->validation_failures = snapshot->validation_failures;
    out_entry->created_sequence = sequence;
    out_entry->updated_sequence = sequence;
    return UMI_STATUS_OK;
}
