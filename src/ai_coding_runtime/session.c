/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ai_coding_runtime/session.c
 *
 * PURPOSE:
 *   Implement compact coding-agent session accounting.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ai_coding_runtime/session.h"

#include <stdio.h>
#include <string.h>

static UmiStatus copy_text(char *destination, size_t capacity, const char *source)
{
    size_t length;

    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    length = strlen(source);
    if (length >= capacity) return UMI_STATUS_CAPACITY_EXCEEDED;

    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

UmiStatus umi_ai_coding_session_init(
    UmiAiCodingSession *session,
    const char *session_id,
    const char *workspace_root)
{
    UmiStatus status;

    if (session == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    (void)memset(session, 0, sizeof(*session));

    status = copy_text(
        session->session_id,
        sizeof(session->session_id),
        session_id);
    if (status != UMI_STATUS_OK) return status;

    status = copy_text(
        session->workspace_root,
        sizeof(session->workspace_root),
        workspace_root);
    if (status != UMI_STATUS_OK) return status;

    session->revision = 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_ai_coding_session_begin_task(
    UmiAiCodingSession *session,
    const char *task_id)
{
    UmiStatus status;

    if (session == NULL || task_id == NULL || task_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (session->active_task_id[0] != '\0') return UMI_STATUS_BUSY;

    status = copy_text(
        session->active_task_id,
        sizeof(session->active_task_id),
        task_id);
    if (status != UMI_STATUS_OK) return status;

    session->task_count += 1U;
    session->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_ai_coding_session_complete_task(
    UmiAiCodingSession *session,
    UmiAiCodingRuntimeState final_state)
{
    if (session == NULL || session->active_task_id[0] == '\0') {
        return UMI_STATUS_INVALID_STATE;
    }

    switch (final_state) {
        case UMI_AI_CODING_RUNTIME_COMPLETED:
            session->completed_count += 1U;
            break;
        case UMI_AI_CODING_RUNTIME_CANCELLED:
            session->cancelled_count += 1U;
            break;
        case UMI_AI_CODING_RUNTIME_FAILED:
            session->failed_count += 1U;
            break;
        default:
            return UMI_STATUS_INVALID_ARGUMENT;
    }

    session->active_task_id[0] = '\0';
    session->revision += 1U;
    return UMI_STATUS_OK;
}
