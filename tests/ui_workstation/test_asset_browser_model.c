#include <stdio.h>
#include "umicom/ui/workstation/asset_browser_model.h"
int main(void) {
    UmiWsAssetBrowserModel m;
    umi_ws_asset_browser_model_init(&m);
    if (umi_ws_asset_browser_model_add(&m, "clip1", "Intro", "file:///intro.mp4", UMI_WS_ASSET_VIDEO, 100U) != UMI_STATUS_OK) return 1;
    if (umi_ws_asset_browser_model_count_kind(&m, UMI_WS_ASSET_VIDEO) != 1U) return 2;
    puts("asset browser model: ok");
    return 0;
}
