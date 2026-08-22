/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/diagnostics.h
 *
 * PURPOSE:
 *   Retain structured context-host diagnostics without coupling to a specific frontend.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_DIAGNOSTICS_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_DIAGNOSTICS_H
#include "umicom/workbench_context_host/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef enum UmiWorkbenchContextHostDiagnosticSeverity {
    UMI_WORKBENCH_CONTEXT_HOST_DIAGNOSTIC_INFO=1,
    UMI_WORKBENCH_CONTEXT_HOST_DIAGNOSTIC_WARNING=2,
    UMI_WORKBENCH_CONTEXT_HOST_DIAGNOSTIC_ERROR=3
} UmiWorkbenchContextHostDiagnosticSeverity;
typedef struct UmiWorkbenchContextHostDiagnostic {
    char diagnostic_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    UmiWorkbenchContextHostDiagnosticSeverity severity;
    char source[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    char message[UMI_WORKBENCH_CONTEXT_HOST_TEXT_CAPACITY];
    char endpoint_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    uint64_t timestamp_ms;
} UmiWorkbenchContextHostDiagnostic;
typedef struct UmiWorkbenchContextHostDiagnostics {
    UmiWorkbenchContextHostDiagnostic items[UMI_WORKBENCH_CONTEXT_HOST_MAX_DIAGNOSTICS];
    size_t count;
    uint64_t dropped_count;
    uint64_t revision;
} UmiWorkbenchContextHostDiagnostics;
void umi_workbench_context_host_diagnostics_init(UmiWorkbenchContextHostDiagnostics *diagnostics);
UmiStatus umi_workbench_context_host_diagnostics_add(
    UmiWorkbenchContextHostDiagnostics *diagnostics,
    const UmiWorkbenchContextHostDiagnostic *diagnostic);
void umi_workbench_context_host_diagnostics_clear(UmiWorkbenchContextHostDiagnostics *diagnostics);
#ifdef __cplusplus
}
#endif
#endif
