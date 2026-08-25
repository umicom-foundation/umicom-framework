/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/workstation/dock_preview.h
 *
 * PURPOSE:
 *   Define toolkit-neutral dock preview geometry for visual drop-target overlays.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_WORKSTATION_DOCK_PREVIEW_H
#define UMICOM_UI_WORKSTATION_DOCK_PREVIEW_H

#include "umicom/ui/workstation/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWsDockPreview {
    UmiWsDockRegion region;
    UmiUiRect bounds;
    double opacity;
    bool visible;
} UmiWsDockPreview;

UmiWsDockPreview umi_ws_dock_preview_compute(UmiUiRect host, UmiWsDockRegion region, double split_fraction);

#ifdef __cplusplus
}
#endif

#endif
