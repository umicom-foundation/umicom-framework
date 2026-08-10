/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/diagnostics/snapshot.h
 *
 * PURPOSE:
 *   Capture one immutable process observability summary from metrics, traces,
 *   audit, readiness and operational events.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DIAGNOSTICS_SNAPSHOT_H
#define UMICOM_DIAGNOSTICS_SNAPSHOT_H

#include <stddef.h>
#include "umicom/base/status.h"
#include "umicom/diagnostics/metrics.h"
#include "umicom/diagnostics/trace.h"
#include "umicom/diagnostics/audit.h"
#include "umicom/diagnostics/readiness.h"
#include "umicom/diagnostics/event_log.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiObservabilitySnapshot { size_t metrics; size_t spans; size_t audit_records; size_t readiness_checks; size_t operational_events; int ready; } UmiObservabilitySnapshot;
UmiStatus umi_observability_snapshot_capture(const UmiMetricsRegistry *metrics,
                                              const UmiTraceStore *traces,
                                              const UmiAuditLog *audit,
                                              const UmiReadinessRegistry *readiness,
                                              const UmiOperationalEventLog *events,
                                              UmiObservabilitySnapshot *out_snapshot);
#ifdef __cplusplus
}
#endif

#endif
