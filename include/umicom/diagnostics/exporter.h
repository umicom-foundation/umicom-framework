/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/diagnostics/exporter.h
 *
 * PURPOSE:
 *   Define a stable exporter boundary for metrics, traces and audit records with
 *   explicit ownership and flush semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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
typedef struct UmiObservabilityExporterV1 {
    uint32_t structure_size;
    uint32_t abi_version;
    void *instance;
    UmiStatus (*metric)(void *instance, const UmiMetricSnapshot *metric);
    UmiStatus (*span)(void *instance, const UmiTraceSpan *span);
    UmiStatus (*audit)(void *instance, const UmiAuditRecord *record);
    UmiStatus (*flush)(void *instance);
    void (*destroy)(void *instance);
} UmiObservabilityExporterV1;
UmiStatus umi_observability_export_metric(const UmiObservabilityExporterV1 *exporter, const UmiMetricSnapshot *metric);
UmiStatus umi_observability_export_span(const UmiObservabilityExporterV1 *exporter, const UmiTraceSpan *span);
UmiStatus umi_observability_export_audit(const UmiObservabilityExporterV1 *exporter, const UmiAuditRecord *record);
UmiStatus umi_observability_export_flush(const UmiObservabilityExporterV1 *exporter);
void umi_observability_exporter_dispose(UmiObservabilityExporterV1 *exporter);
#ifdef __cplusplus
}
#endif

#endif
