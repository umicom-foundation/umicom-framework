/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/qt6/quick_access.h
 *
 * PURPOSE:
 *   Render command/file/symbol/settings quick access using a Qt6 search control.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_QT6_QUICK_ACCESS_H
#define UMICOM_UI_QT6_QUICK_ACCESS_H

#include "umicom/ui/qt6/adapter.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the qt6 quick access descriptor operation used by this module and its client
 * applications.
 */
const UmiQt6SurfaceDescriptor *umi_qt6_quick_access_descriptor(void);
/**
 * Initialise qt6 quick access from caller-provided values so later operations receive a
 * known state.
 */
UmiQt6WidgetHandle umi_qt6_quick_access_create(const UmiQt6RenderRequest *request);

#ifdef __cplusplus
}
#endif
#endif
