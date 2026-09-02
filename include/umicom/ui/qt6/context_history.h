/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/context_history.h
 *
 * PURPOSE:
 *   Render recent typed context transitions for linked workstation panels.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_CONTEXT_HISTORY_H
#define UMICOM_UI_QT6_CONTEXT_HISTORY_H

#include "umicom/ui/qt6/adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the qt6 context history descriptor operation used by this module and its client
 * applications.
 */
const UmiQt6SurfaceDescriptor *umi_qt6_context_history_descriptor(void);
/**
 * Initialise qt6 context history from caller-provided values so later operations receive a
 * known state.
 */
UmiQt6WidgetHandle umi_qt6_context_history_create(const UmiQt6RenderRequest *request);

#ifdef __cplusplus
}
#endif
#endif
