#include <stdio.h>
#include "umicom/ui/workstation/timeline_model.h"
int main(void) {
    UmiWsTimelineModel t;
    umi_ws_timeline_model_init(&t);
    if (umi_ws_timeline_model_add_track(&t, "v1", "Video 1", UMI_WS_TRACK_VIDEO) != UMI_STATUS_OK) return 1;
    if (umi_ws_timeline_model_add_clip(&t, "c1", "v1", 1000, 5000) != UMI_STATUS_OK) return 2;
    if (umi_ws_timeline_model_seek(&t, 2000) != UMI_STATUS_OK || t.playhead_ms != 2000) return 3;
    puts("timeline model: ok");
    return 0;
}
