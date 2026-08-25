/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/context_inspector.h
 *
 * PURPOSE:
 *   Render the active typed context payload and routing metadata.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_CONTEXT_INSPECTOR_H
#define UMICOM_UI_QT6_CONTEXT_INSPECTOR_H

#include "umicom/ui/qt6/adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

const UmiQt6SurfaceDescriptor *umi_qt6_context_inspector_descriptor(void);
UmiQt6WidgetHandle umi_qt6_context_inspector_create(const UmiQt6RenderRequest *request);

#ifdef __cplusplus
}
#endif
#endif
