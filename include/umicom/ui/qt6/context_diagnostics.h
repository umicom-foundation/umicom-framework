/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/context_diagnostics.h
 *
 * PURPOSE:
 *   Render context-channel routing diagnostics and health in Qt6.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_CONTEXT_DIAGNOSTICS_H
#define UMICOM_UI_QT6_CONTEXT_DIAGNOSTICS_H

#include "umicom/ui/qt6/adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

const UmiQt6SurfaceDescriptor *umi_qt6_context_diagnostics_descriptor(void);
UmiQt6WidgetHandle umi_qt6_context_diagnostics_create(const UmiQt6RenderRequest *request);

#ifdef __cplusplus
}
#endif
#endif
