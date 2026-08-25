#include <stdio.h>
#include "umicom/ui/workstation/media_layouts.h"
int main(void) {
    UmiWsLayoutTemplate l;
    if (umi_ws_media_layout_video_editor(&l) != UMI_STATUS_OK || l.domain != UMI_WS_DOMAIN_VIDEO_EDITOR) return 1;
    if (umi_ws_media_layout_ai_creator(&l) != UMI_STATUS_OK || l.domain != UMI_WS_DOMAIN_AI_CREATOR) return 2;
    if (umi_ws_media_layout_music_studio(&l) != UMI_STATUS_OK || l.domain != UMI_WS_DOMAIN_MUSIC_STUDIO) return 3;
    if (umi_ws_media_layout_video_studio(&l) != UMI_STATUS_OK || l.domain != UMI_WS_DOMAIN_VIDEO_STUDIO) return 4;
    puts("media layouts: ok");
    return 0;
}
