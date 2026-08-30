/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/source_control.h
 *
 * PURPOSE:
 *   Render Framework source-control state as a Qt6 change/history surface.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_SOURCE_CONTROL_H
#define UMICOM_UI_QT6_SOURCE_CONTROL_H

#include "umicom/ui/qt6/adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

const UmiQt6SurfaceDescriptor *umi_qt6_source_control_descriptor(void);
UmiQt6WidgetHandle umi_qt6_source_control_create(const UmiQt6RenderRequest *request);

#ifdef __cplusplus
}
#endif
#endif
