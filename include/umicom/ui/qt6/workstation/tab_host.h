/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/workstation/tab_host.h
 *
 * PURPOSE:
 *   Render semantic tab stacks through a reorderable Qt6 tab host.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_WORKSTATION_TAB_HOST_H
#define UMICOM_UI_QT6_WORKSTATION_TAB_HOST_H

#include "umicom/ui/qt6/adapter.h"
#include "umicom/ui/workstation/tab_stack.h"

#ifdef __cplusplus
extern "C" {
#endif

const UmiQt6SurfaceDescriptor *umi_qt6_ws_tab_host_descriptor(void);
UmiQt6WidgetHandle umi_qt6_ws_tab_host_create(const UmiWsTabStack *stack);
UmiStatus umi_qt6_ws_tab_host_append(UmiQt6WidgetHandle host, const char *label, UmiQt6WidgetHandle child);

#ifdef __cplusplus
}
#endif
#endif
