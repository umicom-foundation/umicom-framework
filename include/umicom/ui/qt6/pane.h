/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/pane.h
 *
 * PURPOSE:
 *   Render canonical pane instances as Qt6 dock-widget capable surfaces.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_PANE_H
#define UMICOM_UI_QT6_PANE_H

#include "umicom/ui/qt6/adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the qt6 pane descriptor operation used by this module and its client
 * applications.
 */
const UmiQt6SurfaceDescriptor *umi_qt6_pane_descriptor(void);
/**
 * Initialise qt6 pane from caller-provided values so later operations receive a known
 * state.
 */
UmiQt6WidgetHandle umi_qt6_pane_create(const UmiQt6RenderRequest *request);

#ifdef __cplusplus
}
#endif
#endif
