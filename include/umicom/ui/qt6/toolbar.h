/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/toolbar.h
 *
 * PURPOSE:
 *   Render generic Framework toolbar models through Qt6.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_TOOLBAR_H
#define UMICOM_UI_QT6_TOOLBAR_H

#include "umicom/ui/qt6/adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the qt6 toolbar descriptor operation used by this module and its client
 * applications.
 */
const UmiQt6SurfaceDescriptor *umi_qt6_toolbar_descriptor(void);
/**
 * Initialise qt6 toolbar from caller-provided values so later operations receive a known
 * state.
 */
UmiQt6WidgetHandle umi_qt6_toolbar_create(const UmiQt6RenderRequest *request);

#ifdef __cplusplus
}
#endif
#endif
