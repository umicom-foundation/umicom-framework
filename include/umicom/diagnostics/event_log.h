/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/diagnostics/event_log.h
 *
 * PURPOSE:
 *   Define a bounded operational event log for lifecycle, adapter, recovery and
 *   supervision evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DIAGNOSTICS_EVENT_LOG_H
#define UMICOM_DIAGNOSTICS_EVENT_LOG_H

#include <stddef.h>
#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/diagnostics/diagnostic.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_OPERATIONAL_EVENT_CATEGORY_CAPACITY 96U
#define UMI_OPERATIONAL_EVENT_MESSAGE_CAPACITY 320U
#define UMI_OPERATIONAL_EVENT_MAX 4096U
/**
 * Represent the operational event data shared with callers of this public contract.
 */
typedef struct UmiOperationalEvent { uint64_t sequence; uint64_t timestamp_ns; uint64_t correlation_id; UmiDiagnosticSeverity severity; char category[UMI_OPERATIONAL_EVENT_CATEGORY_CAPACITY]; char message[UMI_OPERATIONAL_EVENT_MESSAGE_CAPACITY]; } UmiOperationalEvent;
/**
 * Represent the operational event log data shared with callers of this public contract.
 */
typedef struct UmiOperationalEventLog UmiOperationalEventLog;
/**
 * Initialise operational event log from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_operational_event_log_create(UmiOperationalEventLog **out_log);
/**
 * Release or reset state held by operational event log so the same storage can be reused
 * safely.
 */
void umi_operational_event_log_destroy(UmiOperationalEventLog *log);
/**
 * Add operational event log only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_operational_event_log_append(UmiOperationalEventLog *log,
                                           const UmiOperationalEvent *event);
/**
 * Return the number of records represented by operational event log without changing their
 * state.
 */
size_t umi_operational_event_log_count(const UmiOperationalEventLog *log);
/**
 * Find operational event log while leaving the underlying catalogue or model owned by this
 * module.
 */
UmiStatus umi_operational_event_log_at(const UmiOperationalEventLog *log,
                                       size_t index,
                                       UmiOperationalEvent *out_event);
#ifdef __cplusplus
}
#endif

#endif
