/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_timeline_model.c
 *
 * PURPOSE:
 *   Implement the test timeline model behavior for
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
#include "umicom/ui/workstation/timeline_model.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiWsTimelineModel t;
    umi_ws_timeline_model_init(&t);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_timeline_model_add_track(&t, "v1", "Video 1", UMI_WS_TRACK_VIDEO) != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_timeline_model_add_clip(&t, "c1", "v1", 1000, 5000) != UMI_STATUS_OK) return 2;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_timeline_model_seek(&t, 2000) != UMI_STATUS_OK || t.playhead_ms != 2000) return 3;
    puts("timeline model: ok");
    return 0;
}
