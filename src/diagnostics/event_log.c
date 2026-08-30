/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/diagnostics/event_log.c
 *
 * PURPOSE:
 *   Implement chronological operational-event retention with stable sequence
 *   numbers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/diagnostics/event_log.h"

#include <stddef.h>
#include <stdlib.h>
#include "diagnostic_lock.h"
struct UmiOperationalEventLog { UmiOperationalEvent events[UMI_OPERATIONAL_EVENT_MAX]; size_t count; size_t head; uint64_t sequence; UmiDiagnosticLock lock; };
UmiStatus umi_operational_event_log_create(UmiOperationalEventLog **out_log)
{
    UmiOperationalEventLog *log;
    if (out_log == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_log = NULL; log = (UmiOperationalEventLog *)calloc(1U, sizeof(*log)); if (log == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    umi_diagnostic_lock_init(&log->lock);
    log->sequence = 1U; *out_log = log; return UMI_STATUS_OK;
}
void umi_operational_event_log_destroy(UmiOperationalEventLog *log) { if (log != NULL) { free(log); } }
UmiStatus umi_operational_event_log_append(UmiOperationalEventLog *log, const UmiOperationalEvent *event)
{
    size_t index;
    if (log == NULL || event == NULL || event->category[0] == '\0' || event->message[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    umi_diagnostic_lock_acquire(&log->lock);
    if (log->count < UMI_OPERATIONAL_EVENT_MAX) { index = (log->head + log->count) % UMI_OPERATIONAL_EVENT_MAX; ++log->count; }
    else { index = log->head; log->head = (log->head + 1U) % UMI_OPERATIONAL_EVENT_MAX; }
    log->events[index] = *event; log->events[index].sequence = log->sequence++;
    umi_diagnostic_lock_release(&log->lock); return UMI_STATUS_OK;
}
size_t umi_operational_event_log_count(const UmiOperationalEventLog *log)
{ size_t n = 0U; if (log != NULL) { umi_diagnostic_lock_acquire(&log->lock); n = log->count; umi_diagnostic_lock_release(&log->lock); } return n; }
UmiStatus umi_operational_event_log_at(const UmiOperationalEventLog *log, size_t index, UmiOperationalEvent *out_event)
{
    if (log == NULL || out_event == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_diagnostic_lock_acquire(&log->lock);
    if (index >= log->count) { umi_diagnostic_lock_release(&log->lock); return UMI_STATUS_NOT_FOUND; }
    *out_event = log->events[(log->head + index) % UMI_OPERATIONAL_EVENT_MAX]; umi_diagnostic_lock_release(&log->lock); return UMI_STATUS_OK;
}
