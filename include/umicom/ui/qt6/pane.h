/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/pane.h
 *
 * PURPOSE:
 *   Render canonical pane instances as Qt6 dock-widget capable surfaces.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_PANE_H
#define UMICOM_UI_QT6_PANE_H

#include "umicom/ui/qt6/adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

const UmiQt6SurfaceDescriptor *umi_qt6_pane_descriptor(void);
UmiQt6WidgetHandle umi_qt6_pane_create(const UmiQt6RenderRequest *request);

#ifdef __cplusplus
}
#endif
#endif
