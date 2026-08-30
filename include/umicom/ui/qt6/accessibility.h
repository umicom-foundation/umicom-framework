/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/accessibility.h
 *
 * PURPOSE:
 *   Apply Framework accessibility semantics to Qt6 presentation surfaces.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_ACCESSIBILITY_H
#define UMICOM_UI_QT6_ACCESSIBILITY_H

#include "umicom/ui/qt6/adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

const UmiQt6SurfaceDescriptor *umi_qt6_accessibility_descriptor(void);
UmiQt6WidgetHandle umi_qt6_accessibility_create(const UmiQt6RenderRequest *request);

#ifdef __cplusplus
}
#endif
#endif
