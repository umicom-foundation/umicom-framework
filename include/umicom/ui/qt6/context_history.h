/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/context_history.h
 *
 * PURPOSE:
 *   Render recent typed context transitions for linked workstation panels.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_CONTEXT_HISTORY_H
#define UMICOM_UI_QT6_CONTEXT_HISTORY_H

#include "umicom/ui/qt6/adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

const UmiQt6SurfaceDescriptor *umi_qt6_context_history_descriptor(void);
UmiQt6WidgetHandle umi_qt6_context_history_create(const UmiQt6RenderRequest *request);

#ifdef __cplusplus
}
#endif
#endif
