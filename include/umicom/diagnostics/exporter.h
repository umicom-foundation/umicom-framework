/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/diagnostics/exporter.h
 *
 * PURPOSE:
 *   Define a stable exporter boundary for metrics, traces and audit records with
 *   explicit ownership and flush semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DIAGNOSTICS_EXPORTER_H
#define UMICOM_DIAGNOSTICS_EXPORTER_H

#include <stdint.h>
#include "umicom/base/status.h"
#include "umicom/base/version.h"
#include "umicom/diagnostics/metrics.h"
#include "umicom/diagnostics/trace.h"
#include "umicom/diagnostics/audit.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiObservabilityExporter {
    uint32_t structure_size;
    uint32_t abi_version;
    void *instance;
    UmiStatus (*metric)(void *instance, const UmiMetricSnapshot *metric);
    UmiStatus (*span)(void *instance, const UmiTraceSpan *span);
    UmiStatus (*audit)(void *instance, const UmiAuditRecord *record);
    UmiStatus (*flush)(void *instance);
    void (*destroy)(void *instance);
} UmiObservabilityExporter;
UmiStatus umi_observability_export_metric(const UmiObservabilityExporter *exporter, const UmiMetricSnapshot *metric);
UmiStatus umi_observability_export_span(const UmiObservabilityExporter *exporter, const UmiTraceSpan *span);
UmiStatus umi_observability_export_audit(const UmiObservabilityExporter *exporter, const UmiAuditRecord *record);
UmiStatus umi_observability_export_flush(const UmiObservabilityExporter *exporter);
void umi_observability_exporter_dispose(UmiObservabilityExporter *exporter);
#ifdef __cplusplus
}
#endif

#endif
