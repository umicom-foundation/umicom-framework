/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/quick_access.h
 *
 * PURPOSE:
 *   Render command/file/symbol/settings quick access using a Qt6 search control.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_QUICK_ACCESS_H
#define UMICOM_UI_QT6_QUICK_ACCESS_H

#include "umicom/ui/qt6/adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

const UmiQt6SurfaceDescriptor *umi_qt6_quick_access_descriptor(void);
UmiQt6WidgetHandle umi_qt6_quick_access_create(const UmiQt6RenderRequest *request);

#ifdef __cplusplus
}
#endif
#endif
