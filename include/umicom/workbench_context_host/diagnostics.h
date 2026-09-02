/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/diagnostics.h
 *
 * PURPOSE:
 *   Retain structured context-host diagnostics without coupling to a specific frontend.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_DIAGNOSTICS_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_DIAGNOSTICS_H
#include "umicom/workbench_context_host/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * List the named workbench context host diagnostic severity values accepted by this public
 * contract.
 */
typedef enum UmiWorkbenchContextHostDiagnosticSeverity {
    UMI_WORKBENCH_CONTEXT_HOST_DIAGNOSTIC_INFO=1,
    UMI_WORKBENCH_CONTEXT_HOST_DIAGNOSTIC_WARNING=2,
    UMI_WORKBENCH_CONTEXT_HOST_DIAGNOSTIC_ERROR=3
} UmiWorkbenchContextHostDiagnosticSeverity;
/**
 * Represent the workbench context host diagnostic data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchContextHostDiagnostic {
    char diagnostic_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    UmiWorkbenchContextHostDiagnosticSeverity severity;
    char source[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    char message[UMI_WORKBENCH_CONTEXT_HOST_TEXT_CAPACITY];
    char endpoint_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    uint64_t timestamp_ms;
} UmiWorkbenchContextHostDiagnostic;
/**
 * Represent the workbench context host diagnostics data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchContextHostDiagnostics {
    UmiWorkbenchContextHostDiagnostic items[UMI_WORKBENCH_CONTEXT_HOST_MAX_DIAGNOSTICS];
    size_t count;
    uint64_t dropped_count;
    uint64_t revision;
} UmiWorkbenchContextHostDiagnostics;
/**
 * Initialise workbench context host diagnostics from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_host_diagnostics_init(UmiWorkbenchContextHostDiagnostics *diagnostics);
/**
 * Add workbench context host diagnostics only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_workbench_context_host_diagnostics_add(
    UmiWorkbenchContextHostDiagnostics *diagnostics,
    const UmiWorkbenchContextHostDiagnostic *diagnostic);
/**
 * Release or reset state held by workbench context host diagnostics so the same storage
 * can be reused safely.
 */
void umi_workbench_context_host_diagnostics_clear(UmiWorkbenchContextHostDiagnostics *diagnostics);
#ifdef __cplusplus
}
#endif
#endif
