/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/workstation/asset_browser.h
 *
 * PURPOSE:
 *   Render reusable image/video/audio/model/material/project asset browsing.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_WORKSTATION_ASSET_BROWSER_H
#define UMICOM_UI_QT6_WORKSTATION_ASSET_BROWSER_H

#include "umicom/ui/qt6/adapter.h"
#include "umicom/ui/workstation/asset_browser_model.h"

#ifdef __cplusplus
extern "C" {
#endif

const UmiQt6SurfaceDescriptor *umi_qt6_ws_asset_browser_descriptor(void);
UmiQt6WidgetHandle umi_qt6_ws_asset_browser_create(const UmiWsAssetBrowserModel *model);

#ifdef __cplusplus
}
#endif
#endif
