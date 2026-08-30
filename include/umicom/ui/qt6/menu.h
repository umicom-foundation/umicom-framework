/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/menu.h
 *
 * PURPOSE:
 *   Render Framework menu models with Qt6 menu-bar semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_MENU_H
#define UMICOM_UI_QT6_MENU_H

#include "umicom/ui/qt6/adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

const UmiQt6SurfaceDescriptor *umi_qt6_menu_descriptor(void);
UmiQt6WidgetHandle umi_qt6_menu_create(const UmiQt6RenderRequest *request);

#ifdef __cplusplus
}
#endif
#endif
