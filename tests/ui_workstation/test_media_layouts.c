/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_workstation/test_media_layouts.c
 *
 * PURPOSE:
 *   Implement the test media layouts behavior for
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
#include "umicom/ui/workstation/media_layouts.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiWsLayoutTemplate l;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_media_layout_video_editor(&l) != UMI_STATUS_OK || l.domain != UMI_WS_DOMAIN_VIDEO_EDITOR) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_media_layout_ai_creator(&l) != UMI_STATUS_OK || l.domain != UMI_WS_DOMAIN_AI_CREATOR) return 2;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_media_layout_music_studio(&l) != UMI_STATUS_OK || l.domain != UMI_WS_DOMAIN_MUSIC_STUDIO) return 3;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_ws_media_layout_video_studio(&l) != UMI_STATUS_OK || l.domain != UMI_WS_DOMAIN_VIDEO_STUDIO) return 4;
    puts("media layouts: ok");
    return 0;
}
