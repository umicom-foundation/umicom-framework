/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/web/workbench/server_diagnostics.h
 *
 * PURPOSE:
 *   Project canonical web-server state and metrics into a developer-facing
 *   diagnostics snapshot without introducing another server implementation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_WEB_WORKBENCH_SERVER_DIAGNOSTICS_H
#define UMICOM_WEB_WORKBENCH_SERVER_DIAGNOSTICS_H

#include "umicom/web/metrics.h"
#include "umicom/web/server_state.h"
#include "umicom/web/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the web workbench server diagnostics data shared with callers of this public
 * contract.
 */
typedef struct UmiWebWorkbenchServerDiagnostics {
    UmiWebServerPhase phase;
    uint16_t port;
    uint64_t requests;
    uint64_t responses_2xx;
    uint64_t responses_4xx;
    uint64_t responses_5xx;
    uint64_t bytes_in;
    uint64_t bytes_out;
    double error_rate;
    UmiStatus last_status;
    bool healthy;
    char summary[UMI_WEB_WORKBENCH_TEXT_CAPACITY];
    uint64_t revision;
} UmiWebWorkbenchServerDiagnostics;

/**
 * Initialise web workbench server diagnostics from caller-provided values so later
 * operations receive a known state.
 */
void umi_web_workbench_server_diagnostics_init(
    UmiWebWorkbenchServerDiagnostics *diagnostics);
/**
 * Provide the web workbench server diagnostics capture operation used by this module and
 * its client applications.
 */
UmiStatus umi_web_workbench_server_diagnostics_capture(
    UmiWebWorkbenchServerDiagnostics *diagnostics,
    const UmiWebServerState *state,
    const UmiWebMetrics *metrics);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_WEB_WORKBENCH_SERVER_DIAGNOSTICS_H */
