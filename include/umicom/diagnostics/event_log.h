/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/diagnostics/event_log.h
 *
 * PURPOSE:
 *   Define a bounded operational event log for lifecycle, adapter, recovery and
 *   supervision evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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
typedef struct UmiOperationalEvent { uint64_t sequence; uint64_t timestamp_ns; uint64_t correlation_id; UmiDiagnosticSeverity severity; char category[UMI_OPERATIONAL_EVENT_CATEGORY_CAPACITY]; char message[UMI_OPERATIONAL_EVENT_MESSAGE_CAPACITY]; } UmiOperationalEvent;
typedef struct UmiOperationalEventLog UmiOperationalEventLog;
UmiStatus umi_operational_event_log_create(UmiOperationalEventLog **out_log);
void umi_operational_event_log_destroy(UmiOperationalEventLog *log);
UmiStatus umi_operational_event_log_append(UmiOperationalEventLog *log,
                                           const UmiOperationalEvent *event);
size_t umi_operational_event_log_count(const UmiOperationalEventLog *log);
UmiStatus umi_operational_event_log_at(const UmiOperationalEventLog *log,
                                       size_t index,
                                       UmiOperationalEvent *out_event);
#ifdef __cplusplus
}
#endif

#endif
