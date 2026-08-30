/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/media_layouts.h
 *
 * PURPOSE:
 *   Define reusable presets for Video Editor, AI Image/Video Creator, Music Studio and Video Studio applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_MEDIA_LAYOUTS_H
#define UMICOM_UI_WORKSTATION_MEDIA_LAYOUTS_H

#include "umicom/ui/workstation/layout_template.h"

#ifdef __cplusplus
extern "C" {
#endif

UmiStatus umi_ws_media_layout_video_editor(UmiWsLayoutTemplate *layout);
UmiStatus umi_ws_media_layout_ai_creator(UmiWsLayoutTemplate *layout);
UmiStatus umi_ws_media_layout_music_studio(UmiWsLayoutTemplate *layout);
UmiStatus umi_ws_media_layout_video_studio(UmiWsLayoutTemplate *layout);

#ifdef __cplusplus
}
#endif

#endif
