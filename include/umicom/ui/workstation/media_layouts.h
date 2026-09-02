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

/**
 * Provide the ws media layout video editor operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_media_layout_video_editor(UmiWsLayoutTemplate *layout);
/**
 * Provide the ws media layout ai creator operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_media_layout_ai_creator(UmiWsLayoutTemplate *layout);
/**
 * Provide the ws media layout music studio operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_media_layout_music_studio(UmiWsLayoutTemplate *layout);
/**
 * Provide the ws media layout video studio operation used by this module and its client
 * applications.
 */
UmiStatus umi_ws_media_layout_video_studio(UmiWsLayoutTemplate *layout);

#ifdef __cplusplus
}
#endif

#endif
