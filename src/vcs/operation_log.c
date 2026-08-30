/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/vcs/operation_log.c
 *
 * PURPOSE:
 *   Implement the operation log behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework VCS operation journal. Created by Sammy Hegab, Umicom Foundation. MIT. */
#include "umicom/vcs/operation_log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
struct UmiVcsOperationLog { UmiVcsOperation *items; size_t count; size_t capacity; uint64_t next_id; };
static UmiStatus ensure_capacity(UmiVcsOperationLog *log)
{
    UmiVcsOperation *resized; size_t next;
    if (log->count < log->capacity) return UMI_STATUS_OK;
    if (log->capacity >= UMI_VCS_MAX_OPERATIONS) return UMI_STATUS_CAPACITY_EXCEEDED;
    next = log->capacity == 0U ? 32U : log->capacity * 2U;
    if (next > UMI_VCS_MAX_OPERATIONS) next = UMI_VCS_MAX_OPERATIONS;
    resized = realloc(log->items, next * sizeof(log->items[0]));
    if (resized == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    log->items = resized; log->capacity = next; return UMI_STATUS_OK;
}
UmiStatus umi_vcs_operation_log_create(UmiVcsOperationLog **out_log)
{
    UmiVcsOperationLog *log;
    if (out_log == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_log = NULL; log = calloc(1U, sizeof(*log));
    if (log == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    log->capacity = 32U;
    log->items = calloc(log->capacity, sizeof(log->items[0]));
    if (log->items == NULL) { free(log); return UMI_STATUS_OUT_OF_MEMORY; }
    log->next_id = 1U; *out_log = log; return UMI_STATUS_OK;
}
void umi_vcs_operation_log_destroy(UmiVcsOperationLog *log)
{ if (log != NULL) { free(log->items); free(log); } }
void umi_vcs_operation_log_clear(UmiVcsOperationLog *log)
{ if (log != NULL) log->count = 0U; }
UmiStatus umi_vcs_operation_log_begin(UmiVcsOperationLog *log, UmiVcsOperationKind kind, const char *subject, uint64_t *out_operation_id)
{
    UmiVcsOperation *operation;
    if (log == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (log->count >= UMI_VCS_MAX_OPERATIONS) {
        (void)memmove(&log->items[0], &log->items[1], (UMI_VCS_MAX_OPERATIONS - 1U) * sizeof(log->items[0]));
        log->count -= 1U;
    } else {
        UmiStatus status = ensure_capacity(log);
        if (status != UMI_STATUS_OK) return status;
    }
    operation = &log->items[log->count++]; (void)memset(operation, 0, sizeof(*operation));
    operation->operation_id = log->next_id++; operation->kind = kind;
    operation->state = UMI_VCS_OPERATION_RUNNING; operation->started_at = (int64_t)time(NULL);
    (void)snprintf(operation->subject, sizeof(operation->subject), "%s", subject != NULL ? subject : "");
    if (out_operation_id != NULL) *out_operation_id = operation->operation_id;
    return UMI_STATUS_OK;
}
UmiStatus umi_vcs_operation_log_finish(UmiVcsOperationLog *log, uint64_t operation_id, UmiStatus status, const char *summary)
{
    size_t index;
    if (log == NULL || operation_id == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < log->count; ++index) {
        UmiVcsOperation *operation = &log->items[index];
        if (operation->operation_id != operation_id) continue;
        operation->state = status == UMI_STATUS_OK ? UMI_VCS_OPERATION_SUCCEEDED :
            (status == UMI_STATUS_CANCELLED ? UMI_VCS_OPERATION_CANCELLED : UMI_VCS_OPERATION_FAILED);
        operation->status_code = (int)status; operation->finished_at = (int64_t)time(NULL);
        (void)snprintf(operation->summary, sizeof(operation->summary), "%s", summary != NULL ? summary : "");
        return UMI_STATUS_OK;
    }
    return UMI_STATUS_NOT_FOUND;
}
size_t umi_vcs_operation_log_count(const UmiVcsOperationLog *log)
{ return log != NULL ? log->count : 0U; }
const UmiVcsOperation *umi_vcs_operation_log_at(const UmiVcsOperationLog *log, size_t index)
{ return log != NULL && index < log->count ? &log->items[index] : NULL; }
const UmiVcsOperation *umi_vcs_operation_log_find(const UmiVcsOperationLog *log, uint64_t operation_id)
{
    size_t index;
    if (log == NULL) return NULL;
    for (index = 0U; index < log->count; ++index) if (log->items[index].operation_id == operation_id) return &log->items[index];
    return NULL;
}
