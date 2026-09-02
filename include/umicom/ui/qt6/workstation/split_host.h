/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/workstation/split_host.h
 *
 * PURPOSE:
 *   Render semantic split regions through Qt6 splitter geometry.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_WORKSTATION_SPLIT_HOST_H
#define UMICOM_UI_QT6_WORKSTATION_SPLIT_HOST_H

#include "umicom/ui/qt6/adapter.h"
#include "umicom/ui/workstation/split_region.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the qt6 ws split host descriptor operation used by this module and its client
 * applications.
 */
const UmiQt6SurfaceDescriptor *umi_qt6_ws_split_host_descriptor(void);
/**
 * Initialise qt6 ws split host from caller-provided values so later operations receive a
 * known state.
 */
UmiQt6WidgetHandle umi_qt6_ws_split_host_create(const UmiWsSplitRegion *split, UmiQt6WidgetHandle first, UmiQt6WidgetHandle second);

#ifdef __cplusplus
}
#endif
#endif
