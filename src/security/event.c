/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/security/event.c
 *
 * PURPOSE:
 *   Implement a chronological bounded security-event ring with denied-action
 *   counters.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/security/event.h"

#include <stddef.h>
#include <stdlib.h>
#include "umicom/platform/threading.h"
struct UmiSecurityEventLog { UmiSecurityEvent items[UMI_SECURITY_EVENT_MAX]; size_t count; size_t head; uint64_t sequence; size_t denied; UmiMutex *mutex; };
UmiStatus umi_security_event_log_create(UmiSecurityEventLog **out_log)
{
    UmiSecurityEventLog *log; UmiStatus status;
    if (out_log == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_log = NULL; log = (UmiSecurityEventLog *)calloc(1U, sizeof(*log)); if (log == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    status = umi_mutex_create(&log->mutex); if (status != UMI_STATUS_OK) { free(log); return status; }
    log->sequence = 1U; *out_log = log; return UMI_STATUS_OK;
}
void umi_security_event_log_destroy(UmiSecurityEventLog *log)
{ if (log != NULL) { umi_mutex_destroy(log->mutex); free(log); } }
UmiStatus umi_security_event_log_append(UmiSecurityEventLog *log, const UmiSecurityEvent *event)
{
    size_t index; UmiStatus status;
    if (log == NULL || event == NULL || event->action[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_mutex_lock(log->mutex); if (status != UMI_STATUS_OK) return status;
    if (log->count < UMI_SECURITY_EVENT_MAX) { index = (log->head + log->count) % UMI_SECURITY_EVENT_MAX; ++log->count; }
    else { index = log->head; log->head = (log->head + 1U) % UMI_SECURITY_EVENT_MAX; }
    log->items[index] = *event; log->items[index].sequence = log->sequence++;
    if (!event->allowed) ++log->denied;
    (void)umi_mutex_unlock(log->mutex); return UMI_STATUS_OK;
}
size_t umi_security_event_log_count(const UmiSecurityEventLog *log)
{ size_t n = 0U; if (log != NULL && umi_mutex_lock(log->mutex) == UMI_STATUS_OK) { n = log->count; (void)umi_mutex_unlock(log->mutex); } return n; }
UmiStatus umi_security_event_log_at(const UmiSecurityEventLog *log, size_t index, UmiSecurityEvent *out_event)
{
    UmiStatus status; if (log == NULL || out_event == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_mutex_lock(log->mutex); if (status != UMI_STATUS_OK) return status;
    if (index >= log->count) { (void)umi_mutex_unlock(log->mutex); return UMI_STATUS_NOT_FOUND; }
    *out_event = log->items[(log->head + index) % UMI_SECURITY_EVENT_MAX];
    (void)umi_mutex_unlock(log->mutex); return UMI_STATUS_OK;
}
size_t umi_security_event_log_denied_count(const UmiSecurityEventLog *log)
{ size_t n = 0U; if (log != NULL && umi_mutex_lock(log->mutex) == UMI_STATUS_OK) { n = log->denied; (void)umi_mutex_unlock(log->mutex); } return n; }
