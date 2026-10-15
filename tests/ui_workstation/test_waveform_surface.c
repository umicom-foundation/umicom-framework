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
#include "../value_archive/construction_cases.h"
#include "umicom/ui/workstation/waveform_surface.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted waveform surface value. */
static int UmiWsWaveformSurfaceConstructionCases(void)
{
    UmiWsWaveformSurface *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_ws_waveform_surface_init(value, "audio.main", 48000, 2, 480000) == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->waveform_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_ws_waveform_surface_init(value, oversized, 48000, 2, 480000) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_ws_waveform_surface_init(value, value->waveform_id, 48000, 2, 480000) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->waveform_id, "audio.main") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void) {
    if (UmiWsWaveformSurfaceConstructionCases() != 0) return 1;

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
