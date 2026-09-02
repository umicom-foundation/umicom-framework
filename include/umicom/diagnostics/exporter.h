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
/**
 * Represent the observability exporter data shared with callers of this public contract.
 */
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
/**
 * Provide the observability export metric operation used by this module and its client
 * applications.
 */
UmiStatus umi_observability_export_metric(const UmiObservabilityExporter *exporter, const UmiMetricSnapshot *metric);
/**
 * Provide the observability export span operation used by this module and its client
 * applications.
 */
UmiStatus umi_observability_export_span(const UmiObservabilityExporter *exporter, const UmiTraceSpan *span);
/**
 * Provide the observability export audit operation used by this module and its client
 * applications.
 */
UmiStatus umi_observability_export_audit(const UmiObservabilityExporter *exporter, const UmiAuditRecord *record);
/**
 * Provide the observability export flush operation used by this module and its client
 * applications.
 */
UmiStatus umi_observability_export_flush(const UmiObservabilityExporter *exporter);
/**
 * Release or reset state held by observability exporter so the same storage can be reused
 * safely.
 */
void umi_observability_exporter_dispose(UmiObservabilityExporter *exporter);
#ifdef __cplusplus
}
#endif

#endif
