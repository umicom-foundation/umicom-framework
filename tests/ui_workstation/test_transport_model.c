/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_transport_model.c
 *
 * PURPOSE:
 *   Implement the test transport model behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#include "umicom/ui/workstation/transport_model.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiWsTransportModel m;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_transport_model_init(&m, 10000) != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_transport_model_play(&m) != UMI_STATUS_OK || m.state != UMI_WS_TRANSPORT_PLAYING) return 2;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_transport_model_seek(&m, 12000) != UMI_STATUS_OK || m.position_ms != 10000) return 3;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_transport_model_set_rate(&m, 2.0) != UMI_STATUS_OK || m.rate != 2.0) return 4;
    puts("transport model: ok");
    return 0;
}
