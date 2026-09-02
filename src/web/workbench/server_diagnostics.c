/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/web/workbench/server_diagnostics.c
 *
 * PURPOSE:
 *   Implement web-server state/metrics projection and health summaries.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/web/workbench/server_diagnostics.h"

#include <stdio.h>
#include <string.h>

/*
 * Initialise web workbench server diagnostics from caller-provided values so later
 * operations receive a known state.
 */
void umi_web_workbench_server_diagnostics_init(
    UmiWebWorkbenchServerDiagnostics *diagnostics)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (diagnostics == NULL) return;
    memset(diagnostics, 0, sizeof(*diagnostics));
    diagnostics->phase = UMI_WEB_SERVER_STOPPED;
    diagnostics->last_status = UMI_STATUS_OK;
    diagnostics->revision = 1U;
}

/*
 * Provide the web workbench server diagnostics capture operation used by this module and
 * its client applications.
 */
UmiStatus umi_web_workbench_server_diagnostics_capture(
    UmiWebWorkbenchServerDiagnostics *diagnostics,
    const UmiWebServerState *state,
    const UmiWebMetrics *metrics)
{
    uint64_t responses;
    int written;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (diagnostics == NULL || state == NULL || metrics == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    diagnostics->phase = state->phase;
    diagnostics->port = state->port;
    diagnostics->requests = metrics->requests;
    diagnostics->responses_2xx = metrics->responses_2xx;
    diagnostics->responses_4xx = metrics->responses_4xx;
    diagnostics->responses_5xx = metrics->responses_5xx;
    diagnostics->bytes_in = metrics->bytes_in;
    diagnostics->bytes_out = metrics->bytes_out;
    diagnostics->last_status = state->last_status;
    responses = metrics->responses_2xx + metrics->responses_4xx + metrics->responses_5xx;
    diagnostics->error_rate = responses > 0U
        ? (double)(metrics->responses_4xx + metrics->responses_5xx) / (double)responses
        : 0.0;
    diagnostics->healthy = state->phase == UMI_WEB_SERVER_READY &&
        state->last_status == UMI_STATUS_OK && diagnostics->error_rate < 0.25;
    written = snprintf(diagnostics->summary, sizeof(diagnostics->summary),
        "%s on port %u: %llu requests, %.2f%% errors",
        umi_web_server_phase_text(state->phase), (unsigned)state->port,
        (unsigned long long)metrics->requests, diagnostics->error_rate * 100.0);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (written < 0 || (size_t)written >= sizeof(diagnostics->summary)) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    diagnostics->revision++;
    return UMI_STATUS_OK;
}
