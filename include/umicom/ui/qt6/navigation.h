/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/navigation.h
 *
 * PURPOSE:
 *   Render canonical navigation surfaces through Qt6 list/stack semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_NAVIGATION_H
#define UMICOM_UI_QT6_NAVIGATION_H

#include "umicom/ui/qt6/adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

const UmiQt6SurfaceDescriptor *umi_qt6_navigation_descriptor(void);
UmiQt6WidgetHandle umi_qt6_navigation_create(const UmiQt6RenderRequest *request);

#ifdef __cplusplus
}
#endif
#endif
