/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_canvas_surface.c
 *
 * PURPOSE:
 *   Implement the test canvas surface behavior for
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
#include "umicom/ui/workstation/canvas_surface.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted canvas surface value. */
static int UmiWsCanvasSurfaceConstructionCases(void)
{
    UmiWsCanvasSurface *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_ws_canvas_surface_init(value, "kitchen.plan") == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->canvas_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_ws_canvas_surface_init(value, oversized) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_ws_canvas_surface_init(value, value->canvas_id) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->canvas_id, "kitchen.plan") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void) {
    if (UmiWsCanvasSurfaceConstructionCases() != 0) return 1;

    UmiWsCanvasSurface c;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_canvas_surface_init(&c, "kitchen.plan") != UMI_STATUS_OK) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_ws_canvas_surface_set_zoom(&c, 2.0) != 2.0) return 2;
    umi_ws_canvas_surface_pan(&c, 10.0, -5.0);
    umi_ws_canvas_surface_set_grid(&c, 20, true);
    /* Apply this branch only when its contract condition is satisfied. */
    if (c.pan_x != 10.0 || c.grid_size != 20) return 3;
    puts("canvas surface: ok");
    return 0;
}
