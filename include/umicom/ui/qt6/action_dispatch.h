/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/action_dispatch.h
 *
 * PURPOSE:
 *   Bind Framework command/action dispatch to Qt6 presentation actions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_ACTION_DISPATCH_H
#define UMICOM_UI_QT6_ACTION_DISPATCH_H

#include "umicom/ui/qt6/adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the qt6 action dispatch descriptor operation used by this module and its client
 * applications.
 */
const UmiQt6SurfaceDescriptor *umi_qt6_action_dispatch_descriptor(void);
/**
 * Initialise qt6 action dispatch from caller-provided values so later operations receive a
 * known state.
 */
UmiQt6WidgetHandle umi_qt6_action_dispatch_create(const UmiQt6RenderRequest *request);

#ifdef __cplusplus
}
#endif
#endif
