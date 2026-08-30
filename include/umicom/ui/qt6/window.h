/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/window.h
 *
 * PURPOSE:
 *   Render Framework window semantics using Qt6 top-level/main-window facilities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_WINDOW_H
#define UMICOM_UI_QT6_WINDOW_H

#include "umicom/ui/qt6/adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

const UmiQt6SurfaceDescriptor *umi_qt6_window_descriptor(void);
UmiQt6WidgetHandle umi_qt6_window_create(const UmiQt6RenderRequest *request);

#ifdef __cplusplus
}
#endif
#endif
