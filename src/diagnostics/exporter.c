/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/diagnostics/exporter.c
 *
 * PURPOSE:
 *   Validate and invoke versioned observability exporter function tables.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/diagnostics/exporter.h"

#include <stddef.h>
#include <string.h>
/* Check that exporter satisfies its contract before another service relies on it. */
static int exporter_valid(const UmiObservabilityExporter *exporter)
{ return exporter != NULL && exporter->structure_size >= sizeof(*exporter) && exporter->abi_version == UMICOM_FRAMEWORK_ABI_VERSION; }
/*
 * Provide the observability export metric operation used by this module and its client
 * applications.
 */
UmiStatus umi_observability_export_metric(const UmiObservabilityExporter *exporter, const UmiMetricSnapshot *metric)
{ return !exporter_valid(exporter) || exporter->metric == NULL || metric == NULL ? UMI_STATUS_INVALID_ARGUMENT : exporter->metric(exporter->instance, metric); }
/*
 * Provide the observability export span operation used by this module and its client
 * applications.
 */
UmiStatus umi_observability_export_span(const UmiObservabilityExporter *exporter, const UmiTraceSpan *span)
{ return !exporter_valid(exporter) || exporter->span == NULL || span == NULL ? UMI_STATUS_INVALID_ARGUMENT : exporter->span(exporter->instance, span); }
/*
 * Provide the observability export audit operation used by this module and its client
 * applications.
 */
UmiStatus umi_observability_export_audit(const UmiObservabilityExporter *exporter, const UmiAuditRecord *record)
{ return !exporter_valid(exporter) || exporter->audit == NULL || record == NULL ? UMI_STATUS_INVALID_ARGUMENT : exporter->audit(exporter->instance, record); }
/*
 * Provide the observability export flush operation used by this module and its client
 * applications.
 */
UmiStatus umi_observability_export_flush(const UmiObservabilityExporter *exporter)
{ return !exporter_valid(exporter) || exporter->flush == NULL ? UMI_STATUS_INVALID_ARGUMENT : exporter->flush(exporter->instance); }
/*
 * Release or reset state held by observability exporter so the same storage can be reused
 * safely.
 */
void umi_observability_exporter_dispose(UmiObservabilityExporter *exporter)
{ /* Protect caller-owned memory by checking that required state is available before it is used. */ if (exporter == NULL) return; /* Protect caller-owned memory by checking that required state is available before it is used. */ if (exporter->destroy != NULL) exporter->destroy(exporter->instance); (void)memset(exporter, 0, sizeof(*exporter)); }
