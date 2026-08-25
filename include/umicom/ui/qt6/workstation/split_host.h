/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/workstation/split_host.h
 *
 * PURPOSE:
 *   Render semantic split regions through Qt6 splitter geometry.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_WORKSTATION_SPLIT_HOST_H
#define UMICOM_UI_QT6_WORKSTATION_SPLIT_HOST_H

#include "umicom/ui/qt6/adapter.h"
#include "umicom/ui/workstation/split_region.h"

#ifdef __cplusplus
extern "C" {
#endif

const UmiQt6SurfaceDescriptor *umi_qt6_ws_split_host_descriptor(void);
UmiQt6WidgetHandle umi_qt6_ws_split_host_create(const UmiWsSplitRegion *split, UmiQt6WidgetHandle first, UmiQt6WidgetHandle second);

#ifdef __cplusplus
}
#endif
#endif
