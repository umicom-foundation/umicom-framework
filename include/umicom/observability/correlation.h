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
/**
 * Represent the operations correlation summary data shared with callers of this public
 * contract.
 */
typedef struct UmiOperationsCorrelationSummary { uint64_t correlation_id; size_t spans; size_t failed_spans; size_t audit_records; size_t denied_or_failed_audits; size_t operational_events; size_t error_events; } UmiOperationsCorrelationSummary;
/**
 * Provide the operations correlate operation used by this module and its client
 * applications.
 */
UmiStatus umi_operations_correlate(uint64_t correlation_id,const UmiTraceStore *traces,const UmiAuditLog *audit,const UmiOperationalEventLog *events,UmiOperationsCorrelationSummary *out_summary);
#endif
