/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/context_status.h
 *
 * PURPOSE:
 *   Render linked-context state in a compact Qt6 status surface.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_CONTEXT_STATUS_H
#define UMICOM_UI_QT6_CONTEXT_STATUS_H

#include "umicom/ui/qt6/adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the qt6 context status descriptor operation used by this module and its client
 * applications.
 */
const UmiQt6SurfaceDescriptor *umi_qt6_context_status_descriptor(void);
/**
 * Initialise qt6 context status from caller-provided values so later operations receive a
 * known state.
 */
UmiQt6WidgetHandle umi_qt6_context_status_create(const UmiQt6RenderRequest *request);

#ifdef __cplusplus
}
#endif
#endif
