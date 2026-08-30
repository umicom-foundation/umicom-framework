/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/activity_bar.h
 *
 * PURPOSE:
 *   Render Framework activities as a compact Qt6 tool rail.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_ACTIVITY_BAR_H
#define UMICOM_UI_QT6_ACTIVITY_BAR_H

#include "umicom/ui/qt6/adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

const UmiQt6SurfaceDescriptor *umi_qt6_activity_bar_descriptor(void);
UmiQt6WidgetHandle umi_qt6_activity_bar_create(const UmiQt6RenderRequest *request);

#ifdef __cplusplus
}
#endif
#endif
