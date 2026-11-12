/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/workstation/asset_browser.h
 *
 * PURPOSE:
 *   Render a reusable asset-browser gallery for images, video, audio, models, materials, presets and project resources.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_WORKSTATION_ASSET_BROWSER_H
#define UMICOM_UI_GTK4_WORKSTATION_ASSET_BROWSER_H

#include <gtk/gtk.h>
#include "umicom/ui/workstation/asset_browser_model.h"

/* This API is implemented by the C23 Framework. Keep its external symbols
 * in the C ABI when a C++ application includes this header directly. */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialise gtk4 ws asset browser from caller-provided values so later operations receive
 * a known state.
 */
GtkWidget *umi_gtk4_ws_asset_browser_create(const UmiWsAssetBrowserModel *model);


#ifdef __cplusplus
}
#endif
#endif
