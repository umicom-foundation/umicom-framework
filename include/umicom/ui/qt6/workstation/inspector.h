/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/workstation/inspector.h
 *
 * PURPOSE:
 *   Render generic editable/read-only property inspection state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_WORKSTATION_INSPECTOR_H
#define UMICOM_UI_QT6_WORKSTATION_INSPECTOR_H

#include "umicom/ui/qt6/adapter.h"
#include "umicom/ui/workstation/inspector_model.h"

#ifdef __cplusplus
extern "C" {
#endif

const UmiQt6SurfaceDescriptor *umi_qt6_ws_inspector_descriptor(void);
UmiQt6WidgetHandle umi_qt6_ws_inspector_create(const UmiWsInspectorModel *model);

#ifdef __cplusplus
}
#endif
#endif
