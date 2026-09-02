/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/controls.h
 *
 * PURPOSE:
 *   Render canonical controls as Qt6 buttons, entries, selectors and toggles.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_CONTROLS_H
#define UMICOM_UI_QT6_CONTROLS_H

#include "umicom/ui/qt6/adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the qt6 controls descriptor operation used by this module and its client
 * applications.
 */
const UmiQt6SurfaceDescriptor *umi_qt6_controls_descriptor(void);
/**
 * Initialise qt6 controls from caller-provided values so later operations receive a known
 * state.
 */
UmiQt6WidgetHandle umi_qt6_controls_create(const UmiQt6RenderRequest *request);

#ifdef __cplusplus
}
#endif
#endif
