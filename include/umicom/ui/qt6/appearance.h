/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/appearance.h
 *
 * PURPOSE:
 *   Project Framework appearance and density state into Qt6 presentation controls.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_APPEARANCE_H
#define UMICOM_UI_QT6_APPEARANCE_H

#include "umicom/ui/qt6/adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

const UmiQt6SurfaceDescriptor *umi_qt6_appearance_descriptor(void);
UmiQt6WidgetHandle umi_qt6_appearance_create(const UmiQt6RenderRequest *request);

#ifdef __cplusplus
}
#endif
#endif
