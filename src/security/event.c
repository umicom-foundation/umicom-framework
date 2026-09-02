/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/security/event.c
 *
 * PURPOSE:
 *   Implement a chronological bounded security-event ring with denied-action
 *   counters.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/security/event.h"

#include <stddef.h>
#include <stdlib.h>
#include "umicom/platform/threading.h"
struct UmiSecurityEventLog { UmiSecurityEvent items[UMI_SECURITY_EVENT_MAX]; size_t count; size_t head; uint64_t sequence; size_t denied; UmiMutex *mutex; };
/*
 * Initialise security event log from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_security_event_log_create(UmiSecurityEventLog **out_log)
{
    UmiSecurityEventLog *log; UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_log == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_log = NULL; log = (UmiSecurityEventLog *)calloc(1U, sizeof(*log)); /* Protect caller-owned memory by checking that required state is available before it is used. */ if (log == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_mutex_create(&log->mutex); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) { free(log); return status; }
    log->sequence = 1U; *out_log = log; return UMI_STATUS_OK;
}
/*
 * Release or reset state held by security event log so the same storage can be reused
 * safely.
 */
void umi_security_event_log_destroy(UmiSecurityEventLog *log)
{ /* Protect caller-owned memory by checking that required state is available before it is used. */ if (log != NULL) { umi_mutex_destroy(log->mutex); free(log); } }
/* Add security event log only after its inputs and available capacity have been checked. */
UmiStatus umi_security_event_log_append(UmiSecurityEventLog *log, const UmiSecurityEvent *event)
{
    size_t index; UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (log == NULL || event == NULL || event->action[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_mutex_lock(log->mutex); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) return status;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (log->count < UMI_SECURITY_EVENT_MAX) { index = (log->head + log->count) % UMI_SECURITY_EVENT_MAX; ++log->count; }
    /* Use this fallback path when the earlier condition does not apply. */
    else { index = log->head; log->head = (log->head + 1U) % UMI_SECURITY_EVENT_MAX; }
    log->items[index] = *event; log->items[index].sequence = log->sequence++;
    /* Apply this operation only while the related capability or state is available. */
    if (!event->allowed) ++log->denied;
    (void)umi_mutex_unlock(log->mutex); return UMI_STATUS_OK;
}
/*
 * Return the number of records represented by security event log without changing their
 * state.
 */
size_t umi_security_event_log_count(const UmiSecurityEventLog *log)
{ size_t n = 0U; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (log != NULL && umi_mutex_lock(log->mutex) == UMI_STATUS_OK) { n = log->count; (void)umi_mutex_unlock(log->mutex); } return n; }
/*
 * Find security event log while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_security_event_log_at(const UmiSecurityEventLog *log, size_t index, UmiSecurityEvent *out_event)
{
    UmiStatus status; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (log == NULL || out_event == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_mutex_lock(log->mutex); /* Preserve the original failure result so the caller can respond to the correct cause. */ if (status != UMI_STATUS_OK) return status;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= log->count) { (void)umi_mutex_unlock(log->mutex); return UMI_STATUS_NOT_FOUND; }
    *out_event = log->items[(log->head + index) % UMI_SECURITY_EVENT_MAX];
    (void)umi_mutex_unlock(log->mutex); return UMI_STATUS_OK;
}
/*
 * Return the number of records represented by security event log denied without changing
 * their state.
 */
size_t umi_security_event_log_denied_count(const UmiSecurityEventLog *log)
{ size_t n = 0U; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (log != NULL && umi_mutex_lock(log->mutex) == UMI_STATUS_OK) { n = log->denied; (void)umi_mutex_unlock(log->mutex); } return n; }
