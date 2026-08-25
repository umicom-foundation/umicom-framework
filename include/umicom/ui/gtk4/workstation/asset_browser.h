/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/gtk4/workstation/asset_browser.h
 *
 * PURPOSE:
 *   Render a reusable asset-browser gallery for images, video, audio, models, materials, presets and project resources.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_GTK4_WORKSTATION_ASSET_BROWSER_H
#define UMICOM_UI_GTK4_WORKSTATION_ASSET_BROWSER_H

#include <gtk/gtk.h>
#include "umicom/ui/workstation/asset_browser_model.h"

GtkWidget *umi_gtk4_ws_asset_browser_create(const UmiWsAssetBrowserModel *model);

#endif
