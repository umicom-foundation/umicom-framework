/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_asset_browser_model.c
 *
 * PURPOSE:
 *   Implement the test asset browser model behavior for
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
#include "umicom/ui/workstation/asset_browser_model.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiWsAssetBrowserModel m;
    umi_ws_asset_browser_model_init(&m);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_asset_browser_model_add(&m, "clip1", "Intro", "file:///intro.mp4", UMI_WS_ASSET_VIDEO, 100U) != UMI_STATUS_OK) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_ws_asset_browser_model_count_kind(&m, UMI_WS_ASSET_VIDEO) != 1U) return 2;
    puts("asset browser model: ok");
    return 0;
}
