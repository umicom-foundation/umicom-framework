/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/observability/correlation.h
 *
 * PURPOSE:
 *   Publish the public correlation contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_OBSERVABILITY_CORRELATION_H
#define UMICOM_OBSERVABILITY_CORRELATION_H
#include "umicom/observability/operations_contract.h"
#include "umicom/diagnostics/trace.h"
#include "umicom/diagnostics/audit.h"
#include "umicom/diagnostics/event_log.h"
typedef struct UmiOperationsCorrelationSummary { uint64_t correlation_id; size_t spans; size_t failed_spans; size_t audit_records; size_t denied_or_failed_audits; size_t operational_events; size_t error_events; } UmiOperationsCorrelationSummary;
UmiStatus umi_operations_correlate(uint64_t correlation_id,const UmiTraceStore *traces,const UmiAuditLog *audit,const UmiOperationalEventLog *events,UmiOperationsCorrelationSummary *out_summary);
#endif
