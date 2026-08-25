/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/context_status.h
 *
 * PURPOSE:
 *   Render linked-context state in a compact Qt6 status surface.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_CONTEXT_STATUS_H
#define UMICOM_UI_QT6_CONTEXT_STATUS_H

#include "umicom/ui/qt6/adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

const UmiQt6SurfaceDescriptor *umi_qt6_context_status_descriptor(void);
UmiQt6WidgetHandle umi_qt6_context_status_create(const UmiQt6RenderRequest *request);

#ifdef __cplusplus
}
#endif
#endif
