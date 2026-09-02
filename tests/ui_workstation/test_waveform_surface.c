/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_waveform_surface.c
 *
 * PURPOSE:
 *   Implement the test waveform surface behavior for
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
#include "umicom/ui/workstation/waveform_surface.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiWsWaveformSurface w;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_waveform_surface_init(&w, "audio.main", 48000, 2, 480000) != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_waveform_surface_select(&w, 100, 1000) != UMI_STATUS_OK) return 2;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_ws_waveform_surface_set_vertical_scale(&w, 50.0) != 16.0) return 3;
    puts("waveform surface: ok");
    return 0;
}
