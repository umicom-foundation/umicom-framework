/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/action_dispatch.h
 *
 * PURPOSE:
 *   Bind Framework command/action dispatch to Qt6 presentation actions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_ACTION_DISPATCH_H
#define UMICOM_UI_QT6_ACTION_DISPATCH_H

#include "umicom/ui/qt6/adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

const UmiQt6SurfaceDescriptor *umi_qt6_action_dispatch_descriptor(void);
UmiQt6WidgetHandle umi_qt6_action_dispatch_create(const UmiQt6RenderRequest *request);

#ifdef __cplusplus
}
#endif
#endif
