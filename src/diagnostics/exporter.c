/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/diagnostics/exporter.c
 *
 * PURPOSE:
 *   Validate and invoke versioned observability exporter function tables.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/diagnostics/exporter.h"

#include <stddef.h>
#include <string.h>
static int exporter_valid(const UmiObservabilityExporter *exporter)
{ return exporter != NULL && exporter->structure_size >= sizeof(*exporter) && exporter->abi_version == UMICOM_FRAMEWORK_ABI_VERSION; }
UmiStatus umi_observability_export_metric(const UmiObservabilityExporter *exporter, const UmiMetricSnapshot *metric)
{ return !exporter_valid(exporter) || exporter->metric == NULL || metric == NULL ? UMI_STATUS_INVALID_ARGUMENT : exporter->metric(exporter->instance, metric); }
UmiStatus umi_observability_export_span(const UmiObservabilityExporter *exporter, const UmiTraceSpan *span)
{ return !exporter_valid(exporter) || exporter->span == NULL || span == NULL ? UMI_STATUS_INVALID_ARGUMENT : exporter->span(exporter->instance, span); }
UmiStatus umi_observability_export_audit(const UmiObservabilityExporter *exporter, const UmiAuditRecord *record)
{ return !exporter_valid(exporter) || exporter->audit == NULL || record == NULL ? UMI_STATUS_INVALID_ARGUMENT : exporter->audit(exporter->instance, record); }
UmiStatus umi_observability_export_flush(const UmiObservabilityExporter *exporter)
{ return !exporter_valid(exporter) || exporter->flush == NULL ? UMI_STATUS_INVALID_ARGUMENT : exporter->flush(exporter->instance); }
void umi_observability_exporter_dispose(UmiObservabilityExporter *exporter)
{ if (exporter == NULL) return; if (exporter->destroy != NULL) exporter->destroy(exporter->instance); (void)memset(exporter, 0, sizeof(*exporter)); }
