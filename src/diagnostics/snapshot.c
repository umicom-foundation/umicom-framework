/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/diagnostics/snapshot.c
 *
 * PURPOSE:
 *   Capture count and readiness summaries without exposing mutable registry
 *   storage.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/diagnostics/snapshot.h"

#include <stddef.h>
UmiStatus umi_observability_snapshot_capture(const UmiMetricsRegistry *metrics, const UmiTraceStore *traces, const UmiAuditLog *audit, const UmiReadinessRegistry *readiness, const UmiOperationalEventLog *events, UmiObservabilitySnapshot *out_snapshot)
{
    if (metrics == NULL || traces == NULL || audit == NULL || readiness == NULL || events == NULL || out_snapshot == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    out_snapshot->metrics = umi_metrics_registry_count(metrics); out_snapshot->spans = umi_trace_store_count(traces); out_snapshot->audit_records = umi_audit_log_count(audit); out_snapshot->readiness_checks = umi_readiness_registry_count(readiness); out_snapshot->operational_events = umi_operational_event_log_count(events); out_snapshot->ready = umi_readiness_registry_ready(readiness); return UMI_STATUS_OK;
}
