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
/* Provide the ensure capacity operation used by this module and its client applications. */
static UmiStatus ensure_capacity(UmiVcsOperationLog *log)
{
    UmiVcsOperation *resized; size_t next;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (log->count < log->capacity) return UMI_STATUS_OK;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (log->capacity >= UMI_VCS_MAX_OPERATIONS) return UMI_STATUS_CAPACITY_EXCEEDED;
    next = log->capacity == 0U ? 32U : log->capacity * 2U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (next > UMI_VCS_MAX_OPERATIONS) next = UMI_VCS_MAX_OPERATIONS;
    resized = realloc(log->items, next * sizeof(log->items[0]));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (resized == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    log->items = resized; log->capacity = next; return UMI_STATUS_OK;
}
/*
 * Initialise vcs operation log from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_vcs_operation_log_create(UmiVcsOperationLog **out_log)
{
    UmiVcsOperationLog *log;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_log == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_log = NULL; log = calloc(1U, sizeof(*log));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (log == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    log->capacity = 32U;
    log->items = calloc(log->capacity, sizeof(log->items[0]));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (log->items == NULL) { free(log); return UMI_STATUS_OUT_OF_MEMORY; }
    log->next_id = 1U; *out_log = log; return UMI_STATUS_OK;
}
/*
 * Release or reset state held by vcs operation log so the same storage can be reused
 * safely.
 */
void umi_vcs_operation_log_destroy(UmiVcsOperationLog *log)
{ /* Protect caller-owned memory by checking that required state is available before it is used. */ if (log != NULL) { free(log->items); free(log); } }
/*
 * Release or reset state held by vcs operation log so the same storage can be reused
 * safely.
 */
void umi_vcs_operation_log_clear(UmiVcsOperationLog *log)
{ /* Protect caller-owned memory by checking that required state is available before it is used. */ if (log != NULL) log->count = 0U; }
/*
 * Provide the vcs operation log begin operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_operation_log_begin(UmiVcsOperationLog *log, UmiVcsOperationKind kind, const char *subject, uint64_t *out_operation_id)
{
    UmiVcsOperation *operation;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (log == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (log->count >= UMI_VCS_MAX_OPERATIONS) {
        (void)memmove(&log->items[0], &log->items[1], (UMI_VCS_MAX_OPERATIONS - 1U) * sizeof(log->items[0]));
        log->count -= 1U;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        UmiStatus status = ensure_capacity(log);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    operation = &log->items[log->count++]; (void)memset(operation, 0, sizeof(*operation));
    operation->operation_id = log->next_id++; operation->kind = kind;
    operation->state = UMI_VCS_OPERATION_RUNNING; operation->started_at = (int64_t)time(NULL);
    (void)snprintf(operation->subject, sizeof(operation->subject), "%s", subject != NULL ? subject : "");
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_operation_id != NULL) *out_operation_id = operation->operation_id;
    return UMI_STATUS_OK;
}
/*
 * Provide the vcs operation log finish operation used by this module and its client
 * applications.
 */
UmiStatus umi_vcs_operation_log_finish(UmiVcsOperationLog *log, uint64_t operation_id, UmiStatus status, const char *summary)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (log == NULL || operation_id == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < log->count; ++index) {
        UmiVcsOperation *operation = &log->items[index];
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (operation->operation_id != operation_id) continue;
        operation->state = status == UMI_STATUS_OK ? UMI_VCS_OPERATION_SUCCEEDED :
            (status == UMI_STATUS_CANCELLED ? UMI_VCS_OPERATION_CANCELLED : UMI_VCS_OPERATION_FAILED);
        operation->status_code = (int)status; operation->finished_at = (int64_t)time(NULL);
        (void)snprintf(operation->summary, sizeof(operation->summary), "%s", summary != NULL ? summary : "");
        return UMI_STATUS_OK;
    }
    return UMI_STATUS_NOT_FOUND;
}
/*
 * Return the number of records represented by vcs operation log without changing their
 * state.
 */
size_t umi_vcs_operation_log_count(const UmiVcsOperationLog *log)
{ return log != NULL ? log->count : 0U; }
/*
 * Find vcs operation log while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiVcsOperation *umi_vcs_operation_log_at(const UmiVcsOperationLog *log, size_t index)
{ return log != NULL && index < log->count ? &log->items[index] : NULL; }
/*
 * Find vcs operation log while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiVcsOperation *umi_vcs_operation_log_find(const UmiVcsOperationLog *log, uint64_t operation_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (log == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < log->count; ++index) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (log->items[index].operation_id == operation_id) return &log->items[index];
    return NULL;
}
