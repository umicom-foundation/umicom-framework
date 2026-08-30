/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/web_workbench/test_server_diagnostics.c
 * PURPOSE: Verify canonical server-state and metric health projection.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/web/workbench/server_diagnostics.h"

int main(void)
{
    UmiWebServerState state;
    UmiWebMetrics metrics;
    UmiWebWorkbenchServerDiagnostics diagnostics;
    umi_web_server_state_init(&state);
    umi_web_metrics_init(&metrics);
    state.phase = UMI_WEB_SERVER_READY;
    state.port = 8080U;
    state.last_status = UMI_STATUS_OK;
    umi_web_metrics_record(&metrics, 100U, 200, 200U);
    umi_web_metrics_record(&metrics, 100U, 404, 100U);
    umi_web_workbench_server_diagnostics_init(&diagnostics);
    assert(umi_web_workbench_server_diagnostics_capture(&diagnostics,
        &state, &metrics) == UMI_STATUS_OK);
    assert(diagnostics.requests == 2U);
    assert(diagnostics.error_rate == 0.5);
    assert(!diagnostics.healthy);
    assert(strstr(diagnostics.summary, "8080") != NULL);
    return 0;
}
