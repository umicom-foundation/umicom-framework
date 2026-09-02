/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_media_viewport.c
 *
 * PURPOSE:
 *   Implement the test media viewport behavior for
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
#include "umicom/ui/workstation/media_viewport.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiWsMediaViewport v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_media_viewport_init(&v, "preview", 3840, 2160) != UMI_STATUS_OK) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_ws_media_viewport_set_zoom(&v, 100.0) != 32.0) return 2;
    umi_ws_media_viewport_set_fit(&v, UMI_WS_VIEWPORT_ONE_TO_ONE);
    /* Apply this branch only when its contract condition is satisfied. */
    if (v.fit != UMI_WS_VIEWPORT_ONE_TO_ONE) return 3;
    puts("media viewport: ok");
    return 0;
}
